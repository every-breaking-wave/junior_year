package com.wave.backend.entity;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableField;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import lombok.Data;

import java.io.Serializable;
import java.math.BigDecimal;

/**
 *
 * @TableName book
 */
@TableName(value ="books")
@Data
public class Book implements Serializable {
    /**
     *
     */
    @TableId(type = IdType.AUTO)
    private Integer id;

    /**
     *
     */
    private String bookName;

    /**
     * 书本分类
     */
    private String category;

    /**
     *
     */
    private String author;

    /**
     *
     */
    private BigDecimal price;

    /**
     * 书本简介
     */
    private String bookDescription;

    /**
     * 书本库存
     */
    private Integer inventory;

    /**
     * 封面图片
     */
    private String cover;

    /**
     *
     */
    private Integer isDeleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;

    @Override
    public boolean equals(Object that) {
        if (this == that) {
            return true;
        }
        if (that == null) {
            return false;
        }
        if (getClass() != that.getClass()) {
            return false;
        }
        Book other = (Book) that;
        return (this.getId() == null ? other.getId() == null : this.getId().equals(other.getId()))
                && (this.getBookName() == null ? other.getBookName() == null : this.getBookName().equals(other.getBookName()))
                && (this.getCategory() == null ? other.getCategory() == null : this.getCategory().equals(other.getCategory()))
                && (this.getAuthor() == null ? other.getAuthor() == null : this.getAuthor().equals(other.getAuthor()))
                && (this.getPrice() == null ? other.getPrice() == null : this.getPrice().equals(other.getPrice()))
                && (this.getBookDescription() == null ? other.getBookDescription() == null : this.getBookDescription().equals(other.getBookDescription()))
                && (this.getInventory() == null ? other.getInventory() == null : this.getInventory().equals(other.getInventory()))
                && (this.getCover() == null ? other.getCover() == null : this.getCover().equals(other.getCover()))
                && (this.getIsDeleted() == null ? other.getIsDeleted() == null : this.getIsDeleted().equals(other.getIsDeleted()));
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((getId() == null) ? 0 : getId().hashCode());
        result = prime * result + ((getBookName() == null) ? 0 : getBookName().hashCode());
        result = prime * result + ((getCategory() == null) ? 0 : getCategory().hashCode());
        result = prime * result + ((getAuthor() == null) ? 0 : getAuthor().hashCode());
        result = prime * result + ((getPrice() == null) ? 0 : getPrice().hashCode());
        result = prime * result + ((getBookDescription() == null) ? 0 : getBookDescription().hashCode());
        result = prime * result + ((getInventory() == null) ? 0 : getInventory().hashCode());
        result = prime * result + ((getCover() == null) ? 0 : getCover().hashCode());
        result = prime * result + ((getIsDeleted() == null) ? 0 : getIsDeleted().hashCode());
        return result;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(getClass().getSimpleName());
        sb.append(" [");
        sb.append("Hash = ").append(hashCode());
        sb.append(", id=").append(id);
        sb.append(", bookName=").append(bookName);
        sb.append(", category=").append(category);
        sb.append(", author=").append(author);
        sb.append(", price=").append(price);
        sb.append(", bookDesciption=").append(bookDescription);
        sb.append(", inventory=").append(inventory);
        sb.append(", cover=").append(cover);
        sb.append(", isDeleted=").append(isDeleted);
        sb.append(", serialVersionUID=").append(serialVersionUID);
        sb.append("]");
        return sb.toString();
    }
}