import scrapy
import datetime
from CrawlModule.items import CrawlModuleItem
end_fdate = '0101'
day_format = '%Y/%m%d'
web_main_entry = 'https://www.chinanews.com.cn'


def yesterday_format(today_format):
    today = datetime.datetime.strptime(today_format, day_format)
    yesterday = today + datetime.timedelta(days=-1)
    return yesterday.strftime(day_format)


def next_url(cur_url: str):
    cur_fdate = '2023/'+cur_url.split('/')[-2]
    next_fdate = yesterday_format(cur_fdate)
    next_url = web_main_entry + '/scroll-news/' + next_fdate \
        + '/news.shtml'
    return next_url


class ChinanewsSpider(scrapy.Spider):
    name = "ChinaNews"
    allowed_domains = ["www.chinanews.com.cn"]
    start_urls = [
        "https://www.chinanews.com.cn/scroll-news/2023/0720/news.shtml"]

    def parse(self, response):
        lis = response.xpath('//div[@class="content_list"]//li[count(*)=3]')
        for li in lis:
            type = li.xpath('./div[1]/a/text()').get()
            if type in ('', '图片', '视频'):
                continue
            url = li.xpath('./div[2]/a/@href').get()
            url = web_main_entry + url
            title = li.xpath('./div[2]/a/text()').get()
            date = li.xpath('./div[3]/text()').get()
            item = CrawlModuleItem()
            item['title'] = title
            item['date'] = date
            request = scrapy.Request(url, callback=self.parse_ctx)
            request.meta['item'] = item
            yield request
        if response.url.split('/')[-2] != end_fdate:
            request = scrapy.Request(next_url(response.url))
            yield request

    def parse_ctx(self, response):
        pstr = ''.join(response.xpath(
            '//div[@class="left_zw"]/p/text()').getall())
        item = response.meta['item']
        item['content'] = pstr
        yield item
