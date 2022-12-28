package com.wave.backend.model.request;

import lombok.Data;

import java.io.Serializable;
@Data
public class updateCartItemRequest implements Serializable {

    private static final long serialVersionUID = -2472450424831582877L;

    private Integer bookId;

    private Integer count;

    private Integer userId;
}
