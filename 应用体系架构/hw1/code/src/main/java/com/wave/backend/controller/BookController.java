package com.wave.backend.controller;

import com.wave.backend.entity.Book;
import com.wave.backend.model.response.SearchBookResponse;
import com.wave.backend.service.BookService;
import lombok.extern.slf4j.Slf4j;
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

//    @PostMapping("/car")
//    public SearchBookResponse getBooksInCar(HttpServletRequest request){
//        Cookie[] cookies =  request.getCookies();
//        if(cookies == null)
//            return null;
//        for (Cookie cookie : cookies) {
//            String cookieName = cookie.getName();
//            log.info(String.valueOf(cookie));
//        }
//        return null;
//
//    }
}
