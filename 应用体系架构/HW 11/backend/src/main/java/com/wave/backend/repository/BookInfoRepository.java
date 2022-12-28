package com.wave.backend.repository;

import com.wave.backend.entity.BookInfo;
import org.springframework.data.mongodb.repository.MongoRepository;

public interface BookInfoRepository extends MongoRepository<BookInfo, Integer> {
    BookInfo findBookInfoByBookId(Integer bookId);
}
