package com.wave.backend.controller;

import com.wave.backend.entity.Book;
import com.wave.backend.fulltextsearching.SearchFiles;
import com.wave.backend.model.response.SearchBookResponse;
import com.wave.backend.service.BookService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Repository;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.List;


@Slf4j
@CrossOrigin
@RestController
@RequestMapping("/book")
public class BookController {
    @Resource
    private BookService bookService;


    @PostMapping("/getbook/{id}")
    public Book getBook(@PathVariable Integer id){
        return bookService.getBook(id);
    }

    @PostMapping("/getbooks/{id}")
    public List<Book> getBooks(@PathVariable Integer id, HttpServletRequest request) {
        return bookService.getBooks(id,request);
    }

    @PostMapping("/search/{keyword}")
    public SearchBookResponse searchBooks(@PathVariable String keyword){
        return bookService.searchBook(keyword);
    }

    @PostMapping("/update")
    public Boolean updateBookInfo(@RequestBody Book book){
        return bookService.saveBook(book);
    }

    @PostMapping("/delete/{id}")
    public Boolean deleteBook(@PathVariable Integer id){
        return bookService.deleteBookById(id);
    }

    @PostMapping("/search-books/{str}")
    public SearchBookResponse searchBooksByStr(@PathVariable String str){
        return bookService.searchBook(str);
    }

    @PostMapping("/fulltextsearch/{text}")
    public SearchBookResponse fulltextBookSearching(@PathVariable String text){
        return new SearchBookResponse(bookService.fulltextBookSearching(text));
    }

    @PostMapping("/createindex")
    public void  createIndex(){
        bookService.createIndex();
    }
}
