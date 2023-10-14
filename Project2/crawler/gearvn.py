from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException
import time
import re

start_url = 'https://gearvn.com/pages/laptop-van-phong'


option = webdriver.ChromeOptions()
option.add_argument('--disable-extensions')
option.add_experimental_option(
    'excludeSwitches', ['enable-logging'])
option.add_argument("--incognito")
option.add_argument("--headless")
browser = webdriver.Chrome(options=option)


def convert_price(price_txt):
    price_txt = re.sub(r"[^\d]", "", price_txt)
    if not re.search(r"\d", price_txt):
        return 0
    return int(price_txt)

def parse():
    browser.get(start_url)
    time.sleep(1)
    links = []
    names = []
    prices = []
    items = browser.find_element(By.XPATH, '//*[@id="banchay"]/div[3]')
    items = items.find_elements(By.TAG_NAME, 'a')
    brand_links = []
    for item in items:
        brand_link = item.get_attribute('href')
        brand_links.append(brand_link)
    for brand_link in brand_links[:6]:
        try:
            print(brand_link)
            browser.get(brand_link)

            print(1)
            items = browser.find_elements(By.CLASS_NAME, 'product-row')
            print(len(items))
            for item in items:
                links.append(item.find_element(By.TAG_NAME, 'a').get_attribute('href'))
                names.append(item.find_element(By.CLASS_NAME, 'product-row-name').text)
                prices.append(item.find_element(By.CLASS_NAME, 'product-row-sale').text)
        except:
            continue
    prices = [convert_price(price) for price in prices]
    print(len(links))

    print(len(names))
    print(len(prices))

parse()