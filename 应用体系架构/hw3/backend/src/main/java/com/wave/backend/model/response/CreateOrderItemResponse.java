package com.wave.backend.model.response;

import com.wave.backend.constant.CreateOrderItemStatus;
import com.wave.backend.constant.CreateOrderStatus;
import lombok.Data;

@Data
public class CreateOrderItemResponse {

    private CreateOrderItemStatus status;

    private Long id;
}
