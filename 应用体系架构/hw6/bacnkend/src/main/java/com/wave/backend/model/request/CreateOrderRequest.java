package com.wave.backend.model.request;

import lombok.Builder;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

@Data
public class CreateOrderRequest implements Serializable {
   private static final long serialVersionUID = -8648194184235698710L;

   private Integer userId;

}
