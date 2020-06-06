module time_sync(clk, reset, avs_s0_address, avs_s0_read, avs_s0_write, avs_s0_readdata, avs_s0_writedata, pps);

input         clk;                // clock.clk
input         reset;              // reset.reset
    
// Memory mapped read/write slave interface
input             avs_s0_address;     // avs_s0.address
input             avs_s0_read;        // avs_s0.read
input             avs_s0_write;       // avs_s0.write
output reg [31:0] avs_s0_readdata;    // avs_s0.readdata
input      [31:0] avs_s0_writedata;   // avs_s0.writedata

// Time pulse input PPS
input             pps;               

reg        [31:0] counter_up;

always @(posedge clk)
begin
   if (reset | pps) begin
      counter_up <= 31'b0 ;
   end else
   begin
      counter_up <= counter_up + 1;
   end
end

always @(posedge clk)
begin
    if (avs_s0_read) begin
            avs_s0_readdata <= counter_up;
    end
end


endmodule