#Create a simulator object
set ns [new Simulator]

#Define different colors for data flows
$ns color 1 Blue
$ns color 2 Red

#Open the nam trace file
set nf [open out.nam w]
$ns namtrace-all $nf

#Define a 'finish' procedure
proc finish {} {
	global ns nf
	$ns flush-trace
	#Close the trace file
	close $nf
	#Execute nam on the trace file
	exec nam out.nam &
	exit 0
}

#Create four nodes
# set a [$ns node]
# set b [$ns node]
# set c [$ns node]
# set d [$ns node]
# set e [$ns node]
for {set i 0} {$i < 7} {incr i} {
	set n($i) [$ns node]
}

for {set i 0} {$i < 7} {incr i} {
	$ns duplex-link $n($i) $n([expr ($i+1)%7]) 1Mb 10ms DropTail
}

#Create links between the nodes
# $ns duplex-link $a $b 2Mb 10ms DropTail
# $ns duplex-link $a $c 0.2Mb 10ms DropTail
# $ns duplex-link $c $d 0.2Mb 10ms DropTail
# $ns duplex-link $b $d 2Mb 10ms DropTail
# $ns duplex-link $d $e 2Mb 10ms DropTail

# $ns duplex-link-op $n0 $n2 orient right-down
# $ns duplex-link-op $n1 $n2 orient right-up
# $ns duplex-link-op $n2 $n3 orient right

#Monitor the queue for the link between node 2 and node 3
# $ns duplex-link-op $d $e queuePos 0.5

set udp0 [new Agent/UDP]
$udp0 set class_ 1
$ns attach-agent $n(0) $udp0

# Create a CBR traffic source and attach it to udp0
set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ 500
$cbr0 set interval_ 0.005
$cbr0 attach-agent $udp0

set udp1 [new Agent/UDP]
$udp1 set class_ 2
$ns attach-agent $n(0) $udp1

# Create a CBR traffic source and attach it to udp1
set cbr1 [new Application/Traffic/CBR]
$cbr1 set packetSize_ 500
$cbr1 set interval_ 0.005
$cbr1 attach-agent $udp1

#Create a Null agent (a traffic sink) and attach it to node n3
set null0 [new Agent/TCPSink]
$ns attach-agent $n(5) $null0

set null1 [new Agent/TCPSink]
$ns attach-agent $n(5) $null1

#Connect the traffic sources with the traffic sink
$ns connect $udp0 $null0
$ns connect $udp1 $null1
$ns rtproto DV

#Schedule events for the CBR agents
$ns at 1 "$cbr0 start"
# $ns rtmodel-at 1.0 down $n(2) $n(3)
# $ns rtmodel-at 3.0 up $n(2) $n(3)
$ns at 1.0 "$cbr1 start"
$ns at 4.0 "$cbr1 stop"
$ns at 4.5 "$cbr0 stop"
#Call the finish procedure after 5 seconds of simulation time
$ns at 5.0 "finish"

#Run the simulation
$ns run
