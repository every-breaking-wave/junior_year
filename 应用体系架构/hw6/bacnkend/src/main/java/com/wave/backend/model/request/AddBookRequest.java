package com.wave.backend.model.request;

import com.sun.org.apache.xpath.internal.objects.XString;
import lombok.Data;


@Data
public class AddBookRequest {
    private String bookName;
    private String cover;
    private java.math.BigDecimal price;
    private String category;
    private String author;
    private Integer inventory;
}
