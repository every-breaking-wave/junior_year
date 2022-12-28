package com.wave.backend.model.response;

import com.wave.backend.constant.CreateOrderStatus;
import lombok.Data;

@Data
public class CreateOrderResponse {

    private CreateOrderStatus status;

    private Integer id;
}
