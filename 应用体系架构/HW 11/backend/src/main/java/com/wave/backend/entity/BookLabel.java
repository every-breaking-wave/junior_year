package com.wave.backend.entity;

import lombok.Data;
import org.springframework.data.neo4j.core.schema.GeneratedValue;
import org.springframework.data.neo4j.core.schema.Id;
import org.springframework.data.neo4j.core.schema.Node;
import org.springframework.data.neo4j.core.schema.Relationship;

import java.util.HashSet;
import java.util.Set;

@Data
@Node
public class BookLabel {
    @Id @GeneratedValue
    private Long  id;

    private String labelName;

    private Integer labelId;

    public BookLabel(){
    }

    private BookLabel(String labelName){
        this.labelName = labelName;
    }
    @Relationship(type = "RECOM")
    public Set<BookLabel> recoms;

    public void addRecomLabel(BookLabel bookLabel){
        if(recoms == null){
            recoms = new HashSet<>();
        }
        recoms.add(bookLabel);
    }



}
