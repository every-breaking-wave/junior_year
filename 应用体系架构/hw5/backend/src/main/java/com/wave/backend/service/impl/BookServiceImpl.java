package com.wave.backend.service.impl;

import com.alibaba.fastjson.JSONArray;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.wave.backend.dao.BookDao;
import com.wave.backend.entity.Book;
import com.wave.backend.fulltextsearching.ReadWriteFiles;
import com.wave.backend.mapper.BookMapper;
import com.wave.backend.model.response.SearchBookResponse;
import com.wave.backend.service.BookService;
import com.wave.backend.utils.RedisUtil;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.List;

/**
* @author Feng
* @description 针对表【book】的数据库操作Service实现
* @createDate 2022-05-09 23:37:44
*/
@Service
@Slf4j
public class BookServiceImpl extends ServiceImpl<BookMapper, Book>
implements BookService{

    @Resource
    private RedisUtil redisUtil;
    @Resource
    private BookDao bookDao;

    @Override
    public SearchBookResponse searchBook(String searchKey) {
        log.info("Searching books");
        SearchBookResponse searchBookResponse = new SearchBookResponse();
        searchBookResponse.setBookList(bookDao.findBySearchKey(searchKey));
        return searchBookResponse;
    }

    @Override
    public Book getBook(Integer id) {
        Object value = redisUtil.get("{bookId:}"+id);
        if (value != null){
            System.out.println("the book of id: "+ id + "is in redis cache");
            return JSONArray.parseObject(value.toString(), Book.class);
        } else {
            System.out.println("Book: " + id + " is not in Redis");
            System.out.println("Searching Book: " + id + " in DB");
            Book book = bookDao.findById(id);
            redisUtil.set("{bookId:}" + id, JSONArray.toJSON(book));
            return book;
        }
    }


    /**
     * 根据权限展示账单中的书
     */
    @Override
    public List<Book> getBooks(Integer id, HttpServletRequest request) {
        return null;
    }

    @Override
    @Transactional
    public Boolean saveBook(Book book) {
        Object value = redisUtil.get("{bookId:}"+book.getId());
        if(value != null)  {
            log.info("the book of id: "+ book.getId() + "is in redis cache");
            redisUtil.set("{bookId:}" + book.getId(), JSONArray.toJSON(book));
        }
        // redis data should be updated if the book is a new one
        return bookDao.saveOne(book) > 0;
    }

    @Override
    @Transactional
    public Boolean deleteBookById(Integer id) {
        redisUtil.del("{bookId:}" + id);
        Boolean result = bookDao.deleteById(id);
        if (result){
            System.out.println("Book: " + id + " is deleted");
        } else {
            System.out.println("delete failed, Book: " + id + "not exist");
        }
        return result;
    }

    @Override
    public List<Book> fulltextBookSearching(String str) {
        return bookDao.fulltextBookSearching(str);
    }

    @Override
    public void createIndex() {
        List<Book> books = bookDao.getAllBooks();
        for (Book book : books){
            ReadWriteFiles.create_docs_files(book.getId(), book.getBookDescription(),  book.getAuthor(), book.getBookName(), true);
        }
    }
}
