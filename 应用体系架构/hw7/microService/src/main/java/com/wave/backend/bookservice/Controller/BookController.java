package com.wave.backend.bookservice.Controller;

import com.alibaba.fastjson.JSON;

import com.wave.backend.bookservice.DTO.ResultDTO;
import com.wave.backend.bookservice.Repository.BookRepository;
import com.wave.backend.bookservice.entity.Book;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.List;


@Slf4j
@RestController
public class BookController {
    @Resource
    BookRepository bookRepository;
    @GetMapping(value = "/getBookAuthor/{bookName}")
    public ResultDTO getBookAuthorByName(@PathVariable("bookName") String bookName){
//        bookName = "%"+ bookName +"%";
        List<Book> books = bookRepository.findBooksByBookNameLike(bookName);
        List<String> authors = new ArrayList<>();
        for (Book book : books) {
            authors.add(book.getBookName() + "的作者是"+ book.getAuthor());
        }
        return ResultDTO.success(JSON.toJSONString(authors));
    }
}