package com.wave.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.wave.backend.entity.Book;
import com.wave.backend.model.response.SearchBookResponse;

import javax.servlet.http.HttpServletRequest;
import java.util.List;

/**
* @author Feng
* @description 针对表【book】的数据库操作Service
* @createDate 2022-05-09 23:37:44
*/
public interface BookService extends IService<Book> {

    SearchBookResponse searchBook(String searchKey);

    Book getBook(Integer id);

    List<Book> getBooks(Integer id, HttpServletRequest request);
}
