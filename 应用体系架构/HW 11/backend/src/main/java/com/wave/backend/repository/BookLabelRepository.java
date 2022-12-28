package com.wave.backend.repository;

import com.wave.backend.entity.Book;
import com.wave.backend.entity.BookLabel;

import org.springframework.data.neo4j.repository.Neo4jRepository;
import org.springframework.data.neo4j.repository.query.Query;
import org.springframework.stereotype.Repository;

import java.util.List;


public interface BookLabelRepository extends Neo4jRepository<BookLabel, Long> {

    @Query("Match (n:BookLabel {labelName:$labelName})-[:RECOM]->()-[:RECOM]->(m:BookLabel) RETURN m")
    List<BookLabel> getLabelsInTwoSteps(String labelName);

    BookLabel findBookLabelByLabelName(String labelName);
}
