class uart_sb;
    mailbox tx = new();
    mailbox rx = new();

    task txAdd(bit [7:0] data);
    begin
        tx.put(data);
        $write("%dns : txAdd     : Added data %x\n",$time, data);
    end
    endtask

    task rxAdd(bit [7:0] data);
    begin
        rx.put(data);
        $write("%dns : rxAdd     : Added data %x\n",$time, data);
    end
    endtask

    task txCompare(bit [7:0] data);
    begin
        bit [7:0] org_data;
        tx.get(org_data);
        if (data != org_data) begin
            $write("%dns : txCompare : Error : Expected data %x, Got %x\n",
                $time, org_data, data);
        end else begin
            $write("%dns : txCompare : Match : Expected data %x, Got %x\n",
                $time, org_data, data);
        end
    end
    endtask

    task rxCompare(bit [7:0] data);
    begin
        bit [7:0] org_data;
        rx.get(org_data);
        if (data != org_data) begin
            $write("%dns : rxCompare : Error : Expected data %x, Got %x\n",
                $time, org_data, data);
        end else begin
            $write("%dns : rxCompare : Match : Expected data %x, Got %x\n",
                $time, org_data, data);
        end
    end
    endtask
endclass
