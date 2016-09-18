-- create ATLG protocol and its fields
p_myproto = Proto ("RMT_CTRL","Remote control protocol")

cmd_desc = {
  [0]  = "WHO",
  [10] = "LIST VALUES",
  [20] = "READ VALUE",
  [30] = "WRITE VALUE",
  [40] = "DESCRIPTION VALUE",
}

-- List all protocol fields
local f_src = ProtoField.uint16("RMT_CTRL.src", "Src", base.DEC)
local f_dst = ProtoField.uint16("RMT_CTRL.dst", "Dst", base.DEC)
local f_cmd = ProtoField.uint8("RMT_CTRL.cmd", "Cmd", base.DEC)
local f_ack = ProtoField.uint8("RMT_CTRL.ack", "Ack", base.DEC)
local f_size = ProtoField.uint16("RMT_CTRL.size", "Size", base.DEC)
local f_group = ProtoField.uint32("RMT_CTRL.group", "Group", base.DEC)
local f_variable = ProtoField.uint32("RMT_CTRL.variable", "Variable", base.DEC)
local f_status = ProtoField.uint16("RMT_CTRL.status", "Status", base.DEC)
 
p_myproto.fields = {f_src, f_dst, f_cmd, f_ack, f_size, f_group, f_variable, f_status}


 
-- ATLG dissector function
function p_myproto.dissector (buf, pkt, root)
  -- validate packet length is adequate, otherwise quit
  if buf:len() == 0 then return end
  pkt.cols.protocol = p_myproto.name
 
  -- create subtree for ATLG display at buffer 0
  subtree = root:add(p_myproto, buf(0))
    -- description of payload
  subtree:append_text(", Command details here or in the tree below")
  
  -- add protocol fields to subtree
  subtree:add(f_src,   buf(0,2)):append_text(" [Src " .. buf(0,2):uint() .. "]")
  subtree:add(f_dst,   buf(2,2)):append_text(" [Dst " .. buf(2,2):uint() .. "]")
  subtree:add(f_cmd,   buf(4,1)):append_text(" [Cmd " .. cmd_desc[buf(4,1):uint()] .. "]")
  subtree:add(f_ack,   buf(5,1)):append_text(" [Ack " .. buf(5,1):uint() .. "]")
  subtree:add(f_size,  buf(6,2)):append_text(" [Size]")
  if (buf(5,1):uint() == 1) then
	subtree:add(f_status,  buf(8,2)):append_text(" [Status ]")
  else
	if (cmd_desc[buf(4,1):uint()] == "READ VALUE") then
		subtree:add(f_group,  buf(8,4)):append_text(" [Group " .. buf(8,4):string() .. "]")
		subtree:add(f_variable,  buf(12,4)):append_text(" [Variable " .. buf(12,4):string() .. "]")
	end
	if (cmd_desc[buf(4,1):uint()] == "WRITE VALUE") then
		subtree:add(f_group,  buf(8,4)):append_text(" [Group " .. buf(8,4):string() .. "]")
		subtree:add(f_variable,  buf(12,4)):append_text(" [Variable " .. buf(12,4):string() .. "]")
	end
	if (cmd_desc[buf(4,1):uint()] == "DESCRIPTION VALUE") then
		subtree:add(f_group,  buf(8,4)):append_text(" [Group " .. buf(8,4):string() .. "]")
		subtree:add(f_variable,  buf(12,4)):append_text(" [Variable " .. buf(12,4):string() .. "]")
	end
  end

end
 
-- Initialization routine
function p_myproto.init()
end
 
-- register a chained dissector for port 8002
local udp_dissector_table = DissectorTable.get("udp.port")
dissector = udp_dissector_table:get_dissector(6666)
  -- you can call dissector from function p_myproto.dissector above
  -- so that the previous dissector gets called
udp_dissector_table:add(6666, p_myproto)
