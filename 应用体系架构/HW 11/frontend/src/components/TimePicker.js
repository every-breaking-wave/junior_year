import { DatePicker } from 'antd';
import React from 'react';
const { RangePicker } = DatePicker;


const onChange = (value, dateString) => {
    console.log('Selected Time: ', value);
    console.log('Formatted Selected Time: ', dateString);
};

const onOk = (value) => {
    console.log('onOk: ', value);
};

const TimePicker = () => (

    <div>
        <RangePicker
            showTime={{
                format: 'HH:mm',
            }}
            format="YYYY-MM-DD HH:mm"
            onChange={onChange}
            onOk={onOk}
        />
    </div>


);

export default TimePicker;