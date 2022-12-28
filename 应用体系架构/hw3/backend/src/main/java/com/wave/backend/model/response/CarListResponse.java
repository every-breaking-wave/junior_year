package com.wave.backend.model.response;

import com.wave.backend.entity.CartItem;
import lombok.Data;

import java.util.List;


@Data
public class CarListResponse {
    List <CartItem> bookInCarList;
}
