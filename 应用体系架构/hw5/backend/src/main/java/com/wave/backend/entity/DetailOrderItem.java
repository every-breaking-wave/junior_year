package com.wave.backend.entity;

import lombok.Data;

import java.math.BigDecimal;
import java.util.Date;

@Data
public class DetailOrderItem {

    private Integer orderId;

    private Integer num;

    private BigDecimal price;

    private String bookName;

    private Date createTime;
}
