package com.wave.backend.entity;

import com.baomidou.mybatisplus.annotation.*;
import com.github.dreamyoung.mprelation.CascadeType;
import com.github.dreamyoung.mprelation.JoinColumn;
import com.github.dreamyoung.mprelation.OneToMany;
import lombok.Data;

import java.io.Serializable;
import java.util.Arrays;
import java.util.Date;
import java.util.Set;

/**
 * 用户表
 * @TableName user
 */
@TableName(value ="users")
@Data
public class User implements Serializable {
    @TableId(type = IdType.AUTO)
    private Integer id;

    /**
     * 用户昵称
     */

    private String userName;

    private String userAccount;

    private String avatarUrl;

    private String userPassword;

    private String email;


    /**
     * 状态 0-正常
     */
    private Integer userStatus;

    /**
     * 创建时间
     */
    private Date createTime;

    /**
     * 是否删除
     */
    private Integer isDelete;

    /**
     * 用户权限，0：正常，1：管理员
     */
    private Integer userRole;

    /**
     * 性别
     */
    private byte[] gender;


    //一对多
    @TableField(exist = false)
    @OneToMany(cascade = CascadeType.ALL) //一对多默认为延迟加载,即@Lazy/@Lazy(true)/或此时不标注
    @JoinColumn(name="id",referencedColumnName = "user-id")//@TableId与一方相关属性中@TableField名称保持一致时@JoinColumn可省略
    private Set<Order> orders;

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
        User other = (User) that;
        return (this.getId() == null ? other.getId() == null : this.getId().equals(other.getId()))
                && (this.getUserName() == null ? other.getUserName() == null : this.getUserName().equals(other.getUserName()))
                && (this.getUserAccount() == null ? other.getUserAccount() == null : this.getUserAccount().equals(other.getUserAccount()))
                && (this.getAvatarUrl() == null ? other.getAvatarUrl() == null : this.getAvatarUrl().equals(other.getAvatarUrl()))
                && (this.getUserPassword() == null ? other.getUserPassword() == null : this.getUserPassword().equals(other.getUserPassword()))
                && (this.getEmail() == null ? other.getEmail() == null : this.getEmail().equals(other.getEmail()))
                && (this.getUserStatus() == null ? other.getUserStatus() == null : this.getUserStatus().equals(other.getUserStatus()))
                && (this.getCreateTime() == null ? other.getCreateTime() == null : this.getCreateTime().equals(other.getCreateTime()))
                && (this.getIsDelete() == null ? other.getIsDelete() == null : this.getIsDelete().equals(other.getIsDelete()))
                && (this.getUserRole() == null ? other.getUserRole() == null : this.getUserRole().equals(other.getUserRole()))
                && (Arrays.equals(this.getGender(), other.getGender()));
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((getId() == null) ? 0 : getId().hashCode());
        result = prime * result + ((getUserName() == null) ? 0 : getUserName().hashCode());
        result = prime * result + ((getUserAccount() == null) ? 0 : getUserAccount().hashCode());
        result = prime * result + ((getAvatarUrl() == null) ? 0 : getAvatarUrl().hashCode());
        result = prime * result + ((getUserPassword() == null) ? 0 : getUserPassword().hashCode());
        result = prime * result + ((getEmail() == null) ? 0 : getEmail().hashCode());
        result = prime * result + ((getUserStatus() == null) ? 0 : getUserStatus().hashCode());
        result = prime * result + ((getCreateTime() == null) ? 0 : getCreateTime().hashCode());
        result = prime * result + ((getIsDelete() == null) ? 0 : getIsDelete().hashCode());
        result = prime * result + ((getUserRole() == null) ? 0 : getUserRole().hashCode());
        result = prime * result + (Arrays.hashCode(getGender()));
        return result;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(getClass().getSimpleName());
        sb.append(" [");
        sb.append("Hash = ").append(hashCode());
        sb.append(", id=").append(id);
        sb.append(", username=").append(userName);
        sb.append(", userAccount=").append(userAccount);
        sb.append(", avatarUrl=").append(avatarUrl);
        sb.append(", userPassword=").append(userPassword);
        sb.append(", email=").append(email);
        sb.append(", userStatus=").append(userStatus);
        sb.append(", createTime=").append(createTime);
        sb.append(", isDelete=").append(isDelete);
        sb.append(", userRole=").append(userRole);
        sb.append(", gender=").append(gender);
        sb.append(", serialVersionUID=").append(serialVersionUID);
        sb.append("]");
        return sb.toString();
    }
}