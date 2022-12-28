package com.wave.backend.entity;


import lombok.Data;
import org.springframework.data.mongodb.core.mapping.Document;

import javax.persistence.Id;
import java.io.Serializable;

@Data
@Document(collection = "bookinfo")
public class BookInfo implements Serializable {
    /**
     *
     */
    @Id
    private Integer bookId;

    private String iconBase64;

}