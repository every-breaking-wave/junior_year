package com.wave.backend.model;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.io.Serializable;


@Data
public class KafkaMessage implements Serializable {
    private String status;
    private Double price;

    public KafkaMessage(String status, Double price) {
        this.status = status;
        this.price = price;
    }
}
