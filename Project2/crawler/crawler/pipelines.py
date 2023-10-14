import psycopg2
import urllib.parse as urlparse
from dotenv import load_dotenv
import os
load_dotenv()

url = os.getenv('database_url')
url_parsed = urlparse.urlparse(url)

class PostgreSQLPipeline(object):

    def __init__(self, host, port, database, username, password):
        self.host = host
        self.port = port
        self.database = database
        self.username = username
        self.password = password

    @classmethod
    def from_crawler(cls, crawler):
        host = crawler.settings.get('DATABASES')['default']['HOST']
        port = crawler.settings.get('DATABASES')['default']['PORT']
        database = crawler.settings.get('DATABASES')['default']['NAME']
        username = crawler.settings.get('DATABASES')['default']['USER']
        password = crawler.settings.get('DATABASES')['default']['PASSWORD']
        return cls(host, port, database, username, password)

    def open_spider(self, spider):
        self.conn = psycopg2.connect(
        host = url_parsed.hostname,
        port = url_parsed.port,
        user = url_parsed.username,
        password=url_parsed.password,
        database=url_parsed.path[1:]
        )
        # self.conn = psycopg2.connect(
        #     host=self.host,
        #     port=self.port,
        #     database=self.database,
        #     user=self.username,
        #     password=self.password,
        #     client_encoding='UTF8'
        # )
        self.cur = self.conn.cursor()

    def close_spider(self, spider):
        self.cur.close()
        self.conn.close()

    def process_item(self, item, spider):
        print(item)
        print('*'*20)
        sql = "INSERT INTO items (website, name, price, link) VALUES (%s, %s, %s, %s)"
        self.cur.execute(sql, (item['website'],item['name'], item['price'], item['link']))
        self.conn.commit()
        return item
