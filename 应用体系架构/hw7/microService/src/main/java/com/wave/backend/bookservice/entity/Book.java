package com.wave.backend.bookservice.entity;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import lombok.Data;

import javax.persistence.*;
import java.math.BigDecimal;

@Entity
@Table(name = "books")
@Data
@JsonIgnoreProperties(value = {"handler","hibernateLazyInitializer","fieldHandler"})
public class Book {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;
    private String bookName;

    /**
     * 书本分类
     */
    private String category;

    /**
     *
     */
    private String author;

    /**
     *
     */
    private BigDecimal price;

    /**
     * 书本简介
     */
    private String bookDescription;

    /**
     * 书本库存
     */
    private Integer inventory;

    /**
     * 封面图片
     */
    private String cover;

    /**
     *
     */
    private Integer isDeleted;

}
