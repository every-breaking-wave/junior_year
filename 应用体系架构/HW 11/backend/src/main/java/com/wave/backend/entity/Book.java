package com.wave.backend.entity;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableField;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import lombok.Data;
import org.springframework.data.neo4j.core.schema.Node;

import javax.persistence.Transient;
import java.io.Serializable;
import java.math.BigDecimal;

/**
 *
 * @TableName book
 */
@TableName(value ="books")
@Data
public class Book implements Serializable {
    /**
     *
     */
    @TableId(type = IdType.AUTO)
    private Integer id;

    /**
     *
     */
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

//    /**
//     * 封面图片
//     */
//    private String cover;

    /**
     *
     */
    private Integer isDeleted;


    @Transient
    private String cover;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;

}