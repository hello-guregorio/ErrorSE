# Define here the models for your scraped items
#
# See documentation in:
# https://docs.scrapy.org/en/latest/topics/items.html

import scrapy


class CrawlModuleItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    docid = scrapy.Field()
    title = scrapy.Field()
    content = scrapy.Field()
    date = scrapy.Field()
