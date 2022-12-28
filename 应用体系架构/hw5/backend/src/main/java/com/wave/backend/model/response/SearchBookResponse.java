package com.wave.backend.model.response;

import com.wave.backend.entity.Book;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.io.Serializable;
import java.util.List;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class SearchBookResponse implements Serializable {
    private List<Book> bookList;
}
