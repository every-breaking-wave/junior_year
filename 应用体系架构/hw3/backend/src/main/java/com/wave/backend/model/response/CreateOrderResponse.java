package com.wave.backend.model.response;

import com.wave.backend.constant.CreateOrderStatus;
import lombok.Builder;
import lombok.Data;

import java.io.Serializable;

@Data
public class CreateOrderResponse implements Serializable {
    private CreateOrderStatus status;
    private Integer id;
    private Integer userId;
    private Double price;

}
