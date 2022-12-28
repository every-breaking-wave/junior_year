package com.wave.backend.entity;

import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import lombok.Data;

import java.io.Serializable;
import java.math.BigDecimal;

@Data
public class BookInCar implements Serializable {

    @TableId
    private Integer id;

    /**
     *
     */
    private String bookName;
    private String cover;

    /**
     *
     */
    private BigDecimal price;

    /**
     *
     */
    private String author;

    /**
     * categrary
     */
    private String category;

    private Integer countInCar;
}
