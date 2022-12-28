package com.wave.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.wave.backend.controller.UserController;
import com.wave.backend.dao.BookDao;
import com.wave.backend.entity.Book;
import com.wave.backend.mapper.BookMapper;
import com.wave.backend.model.response.SearchBookResponse;
import com.wave.backend.service.BookService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Service;

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
    private UserController userController;

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
        log.info("getting book detail");
        return bookDao.findById(id);
    }

    /**
     * 根据权限展示账单中的书
     */
    @Override
    public List<Book> getBooks(Integer id, HttpServletRequest request) {
        // 判断权限
        if(userController.isAdmin(request)){
            return  null;
        }
        return null;
    }
}
