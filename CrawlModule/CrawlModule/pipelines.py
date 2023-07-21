# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html


# useful for handling different item types with a single interface
from itemadapter import ItemAdapter
import json
from scrapy.exporters import JsonLinesItemExporter

docid = 0


class CrawlModulePipeline:
    def __init__(self) -> None:
        print('开始存储爬取数据')
        self.json_file = open('../data/news.rawdata', 'wb')
        self.count_file = open('../data/newcount.txt', 'w')
        self.json_exporter = JsonLinesItemExporter(
            self.json_file, ensure_ascii=False, encoding='UTF-8')
        self.json_exporter.start_exporting()

    def process_item(self, item, spider):
        global docid
        item['docid'] = docid
        docid += 1
        self.json_exporter.export_item(item)
        return item

    def close_spider(self, spider):
        print(f'爬取完成, 共有{docid+1}条新闻')
        self.count_file.write(str(docid+1)+'\n')
        self.json_exporter.finish_exporting()
        self.json_file.close()
