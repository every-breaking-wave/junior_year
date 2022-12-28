package com.wave.backend.entity;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableField;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import java.io.Serializable;
import java.math.BigDecimal;

import com.github.dreamyoung.mprelation.*;
import lombok.Data;

/**
 *
 * @TableName orderitem
 */
@TableName(value ="order_items")
@Data
public class OrderItem implements Serializable {

    @TableId(type = IdType.AUTO)
    private Integer id;

    private Integer bookId;

    private Integer orderId;

    private Integer number;

    private BigDecimal price;

    //一对一
    @TableField(exist = false)
    @OneToOne //一对多默认为延迟加载,即@Lazy/@Lazy(true)/或此时不标注
    @JoinColumn(name="bookId",referencedColumnName = "id")//@TableId与一方相关属性中@TableField名称保持一致时@JoinColumn可省略
    @Lazy(false)
    private Book book;

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
        OrderItem other = (OrderItem) that;
        return (this.getId() == null ? other.getId() == null : this.getId().equals(other.getId()))
                && (this.getBookId() == null ? other.getBookId() == null : this.getBookId().equals(other.getBookId()))
                && (this.getOrderId() == null ? other.getOrderId() == null : this.getOrderId().equals(other.getOrderId()))
                && (this.getNumber() == null ? other.getNumber() == null : this.getNumber().equals(other.getNumber()))
                && (this.getPrice() == null ? other.getPrice() == null : this.getPrice().equals(other.getPrice()));
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((getId() == null) ? 0 : getId().hashCode());
        result = prime * result + ((getBookId() == null) ? 0 : getBookId().hashCode());
        result = prime * result + ((getOrderId() == null) ? 0 : getOrderId().hashCode());
        result = prime * result + ((getNumber() == null) ? 0 : getNumber().hashCode());
        result = prime * result + ((getPrice() == null) ? 0 : getPrice().hashCode());
        return result;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(getClass().getSimpleName());
        sb.append(" [");
        sb.append("Hash = ").append(hashCode());
        sb.append(", id=").append(id);
        sb.append(", bookId=").append(bookId);
        sb.append(", orderId=").append(orderId);
        sb.append(", num=").append(number);
        sb.append(", price=").append(price);
        sb.append(", serialVersionUID=").append(serialVersionUID);
        sb.append("]");
        return sb.toString();
    }
}
