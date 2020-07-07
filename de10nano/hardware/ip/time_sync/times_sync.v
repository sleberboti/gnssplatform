module time_sync
(
	clk, 
	reset,
	avs_s0_address,
	avs_s0_read,
	avs_s0_write,
	avs_s0_readdata,
	avs_s0_writedata,
	pps
);

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

reg 			  prev_pulse_in;
reg               clr;

/*
always @(posedge clk)
begin
   if (pps_active) begin
      counter_up <= 'b0;
      pps_active <= 'b0;
   end else
   begin
      counter_up <= counter_up + 1'b1;
   end
end

always @(posedge pps)
begin
	pps_active <= 'b1;
end
*/

always @(posedge clk or posedge reset) begin
  if (reset) begin // Note that you need to separate the asynchronous reset and synchronous clear logic (ie, dont do 'if (reset | clr)') 
    counter_up <= 'b0; // Use non-blocking assignment for clocked blocks
  end
  else begin
    if (clr) begin
      counter_up <= 'b0;
    end
    else begin
      counter_up <= counter_up + 1;
    end
  end
end

always @(posedge clk) begin // You need to synchronize your input pulse, Im assuming its synchronous to your clock, otherwise youll need a synchronizer
  if (prev_pulse_in & ~pps) begin
    clr <= 1;
  end
  else begin
    clr <= 0;
  end
  prev_pulse_in <= pps;
end

always @(posedge clk)
begin
    if (avs_s0_read) begin
            avs_s0_readdata <= counter_up;
    end
end


endmodule