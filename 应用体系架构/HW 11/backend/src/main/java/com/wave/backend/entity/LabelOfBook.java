package com.wave.backend.entity;

import lombok.Data;

import javax.persistence.*;
import java.util.Objects;

@Entity
@Data
@Table(name = "label_of_book", schema = "ebook", catalog = "")
public class LabelOfBook {
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Id
    @Column(name = "id", nullable = false)
    private int id;
    @Basic
    @Column(name = "bookId", nullable = false)
    private int bookId;
    @Basic
    @Column(name = "labelId", nullable = false)
    private int labelId;

}
