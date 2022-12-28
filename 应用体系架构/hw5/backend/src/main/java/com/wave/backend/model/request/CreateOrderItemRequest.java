package com.wave.backend.model.request;

import com.wave.backend.entity.CartItem;
import lombok.Data;

import java.io.Serializable;
import java.util.List;

@Data
public class CreateOrderItemRequest implements Serializable {

    private static final long serialVersionUID = -6937919124813537741L;
    private Integer id;

    private List<CartItem> bookList;

    private Integer orderId;

}
