package com.wave.backend.controller;

import com.wave.backend.dao.BookDao;
import com.wave.backend.entity.Book;
import com.wave.backend.entity.BookInfo;
import com.wave.backend.entity.BookLabel;
import com.wave.backend.model.response.SearchBookResponse;
import com.wave.backend.repository.BookInfoRepository;
import com.wave.backend.repository.BookLabelRepository;
import com.wave.backend.service.BookService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletRequest;
import java.util.*;


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


    @PostMapping("/find-by-label/{label}")
    public SearchBookResponse findBooksByLabel(@PathVariable String label){
        return new SearchBookResponse(bookService.findByLabels(label));
    }


    @Resource
    private BookDao bookDao;
    @Resource
    private BookInfoRepository bookInfoRepository;
    @Resource
    private BookLabelRepository bookLabelRepository;
    @PostMapping("/test")
    public List<BookInfo> getBookInfo(){
        List<String> names = new ArrayList<String>(){{
            add("编程");   // 0
            add("儿童");   // 1
            add("科幻");   // 2
            add("推理");   // 3
            add("武侠");   // 4
            add("纪实");   // 5
            add("魔幻");   // 6
            add("名著");   // 7
            add("青春");   // 8
            add("教辅");   // 9
        }};
        List<BookLabel> bookLabels = new ArrayList<>();
        for (int i = 0; i < 10; i++){
            bookLabels.add(new BookLabel());
            bookLabels.get(i).setLabelId(i + 1);
            bookLabels.get(i).setLabelName(names.get(i));
        }
        bookLabelRepository.deleteAll();
        Set<String> h = new HashSet<>(Arrays.asList("a", "b"));

        bookLabels.get(0).addRecomLabel(bookLabels.get(2));
        bookLabels.get(0).addRecomLabel(bookLabels.get(3));

        bookLabels.get(1).addRecomLabel(bookLabels.get(8));

        bookLabels.get(2).addRecomLabel(bookLabels.get(3));
        bookLabels.get(2).addRecomLabel(bookLabels.get(4));

        bookLabels.get(3).addRecomLabel(bookLabels.get(6));


        bookLabels.get(4).addRecomLabel(bookLabels.get(6));
        bookLabels.get(4).addRecomLabel(bookLabels.get(8));

        bookLabels.get(5).addRecomLabel(bookLabels.get(7));

//        bookLabels.get(6).addRecomLabel(bookLabels.get(2));
//
//        bookLabels.get(7).addRecomLabel(bookLabels.get(1));
//
//        bookLabels.get(8).addRecomLabel(bookLabels.get(9));
//
//        bookLabels.get(9).addRecomLabel(bookLabels.get(0));
////


        for (int i = 0; i < 10; i++){
          bookLabelRepository.save(bookLabels.get(i));
        }
        return null;
    }
}
