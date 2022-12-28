package com.wave.backend.service;

import com.sun.org.apache.xml.internal.utils.BoolStack;
import com.wave.backend.dao.BookDao;
import com.wave.backend.dao.daoImpl.BookDaoImpl;
import com.wave.backend.entity.Book;
import com.wave.backend.entity.BookInfo;
import com.wave.backend.entity.Order;
import com.wave.backend.repository.BookInfoRepository;
import org.junit.jupiter.api.Test;
import sun.misc.BASE64Encoder;

import javax.annotation.Resource;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.List;

class OrderServiceTest {
    @Resource
    private BookService bookService;
    @Resource
    private BookDao bookDao;
    @Resource
    private BookInfoRepository bookInfoRepository;
    @Test
    void test() {
        List<Book> books = bookDao.getAllBooks();
        for (Book book : books){
            BookInfo bookInfo = new BookInfo();
            bookInfo.setBookId(book.getId());
            bookInfo.setIconBase64(GetImageStrFromUrl(book.getCover()));
            bookInfoRepository.insert(bookInfo);
        }
    }

    public static String GetImageStrFromUrl(String imgURL) {
        ByteArrayOutputStream data = new ByteArrayOutputStream();
        try {
            // 创建URL
            URL url = new URL(imgURL);
            byte[] by = new byte[1024];
            // 创建链接
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("GET");
            conn.setConnectTimeout(5000);
            InputStream is = conn.getInputStream();
            // 将内容读取内存中
            int len = -1;
            while ((len = is.read(by)) != -1) {
                data.write(by, 0, len);
            }
            // 关闭流
            is.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        // 对字节数组Base64编码
        BASE64Encoder encoder = new BASE64Encoder();
        return encoder.encode(data.toByteArray());
    }
}




