package com.wave.backend.dao;

import com.wave.backend.entity.Book;

import java.util.List;

public interface BookDao {

    Integer saveOne(Book book);

    Boolean ifBookExist(String bookName);

    List<Book> findBySearchKey(String key);

    Book findById(Integer id);

    Boolean deleteById(Integer id);

    List<Book> fulltextBookSearching(String text);

    List<Book> getAllBooks();
}
