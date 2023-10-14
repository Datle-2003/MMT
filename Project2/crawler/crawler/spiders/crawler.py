from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException
import time
import re
import scrapy


class FptShopSpider(scrapy.Spider):
    name = 'fptshop'
    allowed_domains = ['https://fptshop.com.vn']
    start_urls = ['https://fptshop.com.vn/may-tinh-xach-tay']

    def __init__(self):
        self.option = webdriver.ChromeOptions()
        self.option.add_experimental_option('excludeSwitches', ['enable-logging'])
        self.option.add_argument('--headless')
        self.option.add_argument('--no-sandbox')
        self.option.add_argument('--disable-dev-shm-usage')
        self.browser = webdriver.Chrome(options=self.option)
        super(FptShopSpider, self).__init__()

    def convert_price(self, price_txt):
        price_txt = re.sub(r"[^\d]", "", price_txt)
        if not re.search(r"\d", price_txt):
            return 0
        return int(price_txt)

    def extract_info(self, text):
        pattern = r'^(.*?)(\d{1,3}\.\d{3}\.\d{3})'

        match = re.search(pattern, text, re.DOTALL)
        if match:
            name = match.group(1).rstrip('\n')
            price = match.group(2)
            return [name, price]

    def parse(self, response):
        self.browser.get(response.url)
        time.sleep(5)
    # click the show more button repeatly to load the entire content
        while True:
            try:
                show_more_button = WebDriverWait(self.browser, 30).until(EC.element_to_be_clickable(
                    (By.XPATH, '//*[@id="root"]/main/div/div[3]/div[2]/div[3]/div/div[3]/a')))
                show_more_button.click()
            except TimeoutException:
                print('cant show more button')
                break

    # parse the loaded content
        try:
            raw_infos = self.browser.find_elements(
                By.CLASS_NAME, 'cdt-product__info')
            for raw_info in raw_infos:
                name, price = self.extract_info(
                    raw_info.text)  # extract name and price
                link = raw_info.find_element(
                    By.TAG_NAME, 'a').get_attribute('href')
                yield {
                    'website': 'fpt',
                    'name': name,
                    'price': self.convert_price(price),
                    'link': link
                }
        except Exception as e:
            print(str(e))

    def closed(self, reason):
        self.browser.quit()


class TgddSpider(scrapy.Spider):
    name = 'tgdd'
    allowed_domains = ['https://www.thegioididong.com']
    start_urls = ['https://www.thegioididong.com/laptop-ldp']

    def __init__(self):
        self.option = webdriver.ChromeOptions()
        self.option.add_experimental_option(
            'excludeSwitches', ['enable-logging'])
        self.option.add_argument("--incognito")
        self.browser = webdriver.Chrome(options=self.option)
        super(TgddSpider, self).__init__()

    def convert_price(self, price_txt):
        price_txt = re.sub(r"[^\d]", "", price_txt)
        if not re.search(r"\d", price_txt):
            return 0
        return int(price_txt)

    def parse(self, response):
        self.browser.get(response.url)
        brands = self.browser.find_element(By.XPATH, "/html/body/div[7]/div")
        brand_urls = [brand.get_attribute(
            'href') for brand in brands.find_elements(By.TAG_NAME, 'a')]
        for brand_url in brand_urls:
            self.browser.get(brand_url)
            time.sleep(3)
            productls_driver = self.browser.find_element(
                By.CLASS_NAME, "listproduct")

            names = []
            prices = []
            links = []

            for element in productls_driver.find_elements(By.TAG_NAME, 'h3'):
                names.append(element.text)

            for element in productls_driver.find_elements(By.CLASS_NAME, "price"):
                prices.append(self.convert_price(element.text))

            for element in productls_driver.find_elements(By.CLASS_NAME, "main-contain"):
                links.append(element.get_attribute('href'))

            for i in range(len(names)):
                yield {
                    'website': 'thegioididong',
                    'name': names[i],
                    'price': prices[i],
                    'link': links[i]
                }

    def closed(self, reason):
        self.browser.quit()


class DienMayXanhSpider(scrapy.Spider):
    name = 'dienmayxanh'
    allowed_domains = ['https://www.dienmayxanh.com']
    start_urls = ['https://www.dienmayxanh.com/laptop']

    def __init__(self):
        self.option = webdriver.ChromeOptions()
        self.option.add_experimental_option(
            'excludeSwitches', ['enable-logging'])
        self.option.add_argument("--incognito")
        self.browser = webdriver.Chrome(options=self.option)
        super(DienMayXanhSpider, self).__init__()

    def convert_price(self, price_txt):
        price_txt = re.sub(r"[^\d]", "", price_txt)
        if not re.search(r"\d", price_txt):
            return 0
        return int(price_txt)

    def extract_info(self, text):
        name_pattern = r'^(.+)\nRAM'
        name_match = re.search(name_pattern, text, re.MULTILINE)
        name = name_match.group(1)

        price_pattern = r"(\d[\d\.]*)₫"
        prices_match = re.findall(price_pattern, text)

        price = 0
        if len(prices_match) == 1:
            price = prices_match[0]
        else:
            price = prices_match[len(prices_match) - 2]
        price = self.convert_price(price)

        return [name, price]

    def parse(self, response):
        self.browser.get(response.url)
    # click the show more button repeatly to load the entire content
        while True:
            try:
                show_more_button = WebDriverWait(self.browser, 10).until(EC.element_to_be_clickable(
                    (By.XPATH, '//*[@class="cate-main-container"]/section/div[3]/div[2]/a')))
                show_more_button.click()
                time.sleep(3)
            except TimeoutException:
                break
    # parse the loaded content
        try:
            raw_infos = self.browser.find_elements(
                By.CSS_SELECTOR, ".item.__cate_44")
            for raw_info in raw_infos:
                name, price = self.extract_info(raw_info.text)
                link = raw_info.find_element(
                    By.TAG_NAME, 'a').get_attribute('href')
                yield {
                    'website': 'dienmayxanh',
                    'name': name,
                    'price': int(price),
                    'link': link
                }
        except Exception as e:
            print(str(e))

    def closed(self, reason):
        self.browser.quit()


class HoangHaMobileSpider(scrapy.Spider):
    name = 'hoanghamobile'
    allowed_domains = ['https://hoanghamobile.com']
    start_urls = ['https://hoanghamobile.com/laptop']

    def __init__(self):
        self.option = webdriver.ChromeOptions()
        self.option.add_argument('--disable-extensions')
        self.option.add_experimental_option(
            'excludeSwitches', ['enable-logging'])
        self.option.add_argument("--incognito")
        self.browser = webdriver.Chrome(options=self.option)
        super(HoangHaMobileSpider, self).__init__()

    def extract_info(self, text):
        name_pattern = r"Laptop\s(.+?)\n"
        price_pattern = r"(\d{1,3}\,\d{3}\,\d{3})"
        name_match = re.search(name_pattern, text)
        price_matches = re.findall(price_pattern, text)
        name = name_match.group(1)
        name = name.split(" - ")[0]
        price = self.convert_price(price_matches[0])
        return [name, price]

    def convert_price(self, price_txt):
        price_txt = re.sub(r"[^\d]", "", price_txt)
        if not re.search(r"\d", price_txt):
            return 0
        return int(price_txt)

    def parse(self, response):
        self.browser.get(response.url)
        time.sleep(5)

        # close pop-ups
        try:
            WebDriverWait(self.browser, 10).until(EC.element_to_be_clickable(
            (By.CSS_SELECTOR, '.close-modal.icon-minutes'))).click()
        except TimeoutException:
            print("pop-ups not exist")
        while True:
            try:
                show_more_button = WebDriverWait(self.browser, 10).until(
                    EC.element_to_be_clickable((By.XPATH, '//*[@class="container"]/div[2]/a')))
                show_more_button.click()
                time.sleep(3)
            except TimeoutException:
                break

        try:
            content = self.browser.find_element(
                By.CSS_SELECTOR, ".col-content.lts-product")
            raw_infos = content.find_elements(By.CLASS_NAME, 'item')
            for raw_info in raw_infos:
                name, price = self.extract_info(raw_info.text)
                link = raw_info.find_element(By.CLASS_NAME, 'info').find_element(
                    By.TAG_NAME, 'a').get_attribute('href')

                yield {
                    'website': 'hoanghamobile',
                    'name': name,
                    'price': int(price),
                    'link': link
                }
        except Exception as e:
            print(str(e))

    def closed(self, reason):
        self.browser.quit()
        
        
class AnPhatPCSpider(scrapy.Spider):
    name = 'anphatpc'
    start_urls = ['https://www.anphatpc.com.vn/laptop-theo-hang.html']

    def __init__(self):
        self.option = webdriver.ChromeOptions()
        self.option.add_experimental_option('excludeSwitches', ['enable-logging'])
        self.option.add_argument("--incognito")
        self.browser = webdriver.Chrome(options=self.option)
        super(FptShopSpider, self).__init__()

    def convert_price(self, price_txt):
        price_txt = re.sub(r"[^\d]", "", price_txt)
        if not re.search(r"\d", price_txt):
            return 0
        return int(price_txt)

    def parse(self, response):
        brand_links = response.css('#js-filter-height table tbody tr:nth-child(1) td:nth-child(2) div a::attr(href)').getall()

        for brand_link in brand_links:
            yield response.follow(brand_link, callback=self.parse_brand)

    def parse_brand(self, response):
        raw_infos = response.css('section div.product-list-container.bg-white div.p-list-container.d-flex.flex-wrap div.p-text')

        for raw_info in raw_infos:
            name = raw_info.css('a h3::text').get()
            price = raw_info.css('div.price-container span.p-price::text').get()
            link = raw_info.css('a::attr(href)').get()

            name = name.strip() if name else None
            price = self.convert_price(price) if price else 0

            yield {
                'website': 'anphatpc',
                'name': name,
                'price': price,
                'link': link
            }
    def closed(self, reason):
        self.browser.quit()

        
        
        
       
class HacomSpider(scrapy.Spider):
    name = 'hacom'
    start_urls = ['https://hacom.vn/laptop']

    def __init__(self):
        self.option = webdriver.ChromeOptions()
        self.option.add_experimental_option('excludeSwitches', ['enable-logging'])
        self.option.add_argument("--incognito")
        self.browser = webdriver.Chrome(options=self.option)
        super(FptShopSpider, self).__init__()

    def convert_price(self, price_txt):
        price_txt = re.sub(r"[^\d]", "", price_txt)
        if not re.search(r"\d", price_txt):
            return 0
        return int(price_txt)

    def parse(self, response):
        urls = [response.urljoin(f'{response.url}/{i + 1}/') for i in range(10)]

        for url in urls:
            yield scrapy.Request(url, callback=self.parse_page)

    def parse_page(self, response):
        links = response.css('#159 div[data-pid] a::attr(href)').getall()
        names = response.css('#159 div[data-pid] a::text').getall()
        prices = response.css('#159 div[data-pid] span.price::text').getall()

        prices = [self.convert_price(price) for price in prices]

        for name, link, price in zip(names, links, prices):
            yield {
                'name': name,
                'link': link,
                'price': price,
            }

    def closed(self, reason):
        self.browser.quit()

class NguyenKimSpider(scrapy.Spider):
    name = 'nguyenkim'
    start_urls = ['https://www.nguyenkim.com/laptop-may-tinh-xach-tay']

    custom_settings = {
        'DOWNLOAD_DELAY': 1,
        'CONCURRENT_REQUESTS': 1,
        'ROBOTSTXT_OBEY': False,
        'USER_AGENT': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36',
    }

    def convert_price(self, price_txt):
        price_txt = re.sub(r"[^\d]", "", price_txt)
        if not re.search(r"\d", price_txt):
            return 0
        return int(price_txt)

    def parse(self, response):
        urls = [f'{response.url}/page-{i + 1}/' for i in range(4)]

        for url in urls:
            yield scrapy.Request(url, callback=self.parse_page)

    def parse_page(self, response):
        links = response.css('div.item h3 a::attr(href)').getall()
        names = response.css('div.item h3 a::text').getall()
        prices = response.css('div.item span.price strong::text').getall()

        prices = [self.convert_price(price) for price in prices]

        for name, link, price in zip(names, links, prices):
            yield {
                'name': name,
                'link': link,
                'price': price,
            }


class GearVNSpider(scrapy.Spider):
    name = 'gearvn'
    start_urls = ['https://gearvn.com/pages/laptop-van-phong']

    custom_settings = {
        'DOWNLOAD_DELAY': 1,
        'CONCURRENT_REQUESTS': 1,
        'ROBOTSTXT_OBEY': False,
        'USER_AGENT': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.182 Safari/537.36',
    }

    def convert_price(self, price_txt):
        price_txt = re.sub(r"[^\d]", "", price_txt)
        if not re.search(r"\d", price_txt):
            return 0
        return int(price_txt)

    def parse(self, response):
        browser_links = response.css('#banchay div[class*="col-xl-2"] a::attr(href)').getall()

        for browser_link in browser_links[:6]:
            yield scrapy.Request(browser_link, callback=self.parse_page)

    def parse_page(self, response):
        links = response.css('div.product-row a::attr(href)').getall()
        names = response.css('div.product-row .product-row-name::text').getall()
        prices = response.css('div.product-row .product-row-sale::text').getall()

        prices = [self.convert_price(price) for price in prices]

        for name, link, price in zip(names, links, prices):
            yield {
                'name': name,
                'link': link,
                'price': price,
            }
    def closed(self, reason):     
        self.browser.quit()
        
        
        
        
        
        
        
        
        
        
        
        
        
        
