package com.wave.backend.bookservice.Repository;


import com.wave.backend.bookservice.entity.Book;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface BookRepository extends JpaRepository<Book,Integer> {

    List<Book> findBooksByBookNameLike(String bookname);

    Book findBookByAuthor(String str);

}
