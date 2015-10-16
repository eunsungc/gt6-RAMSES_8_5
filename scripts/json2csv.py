import sys
import csv
import json

f = open(sys.argv[1], 'r') 
csv_f = csv.writer(open(sys.argv[2], "wb+"))

# Write CSV Header, If you dont need that, remove this line
#f.writerow(["pk", "model", "codename", "name", "content_type"])
csv_f.writerow(["transferID", 
		"event_type",
		"start_timestamp",
		"end_timestamp",
		"user",
		"filepath",
		"tcp_bufsize",
		"globus_blocksize",
		"nbytes",
		"nstreams",
		"dest",
		"cmd_type",
		"ret_code",
		"mpstat.%usr",
		"mpstat.%nice",
		"mpstat.%sys",
		"mpstat.%iowait",
		"mpstat.%irq",
		"mpstat.%soft",
		"mpstat.%steal",
		"mpstat.%quest",
		"mpstat.%idle",
		"host",
		"volume",
		"dev",
		"tps",
		"BlkRead",
		"BlkWrite",
		"getrusage.ru_utime",
		"getrusage.ru_stime",
		"getrusage.ru_maxrss",
		"getrusage.ru_ixrss",
		"getrusage.ru_idrss",
		"getrusage.ru_isrss",
		"getrusage.ru_minflt",
		"getrusage.ru_majflt",
		"getrusage.ru_nswap",
		"getrusage.ru_inblock",
		"getrusage.ru_oublock",
		"getrusage.ru_msgsnd",
#		"getrusage.ru_msgrcv",
		"getrusage.ru_nsignals",
		"getrusage.ru_nvcsw",
		"getrusage.ru_nivcsw",
		"streams[0].TCPinfo.rcv_ssthresh",
		"streams[0].TCPinfo.rtt",
		"streams[0].TCPinfo.rttvar",
		"streams[0].TCPinfo.snd_ssthresh",
		"streams[0].TCPinfo.snd_cwnd",
		"streams[0].TCPinfo.advmss",
		"streams[0].TCPinfo.reordering",
		"streams[0].TCPinfo.rcv_rtt",
		"streams[0].TCPinfo.rcv_space",
		"streams[0].TCPinfo.total_retrans"
            ])
#
for row in f:
    try:
        json_log = json.loads(row) # loads() convert a string while load() conver a file
    except ValueError:
        print("JSON converting error. Move to the next line.")
        continue
#    filepath = json_log["file"]
    filepath = json_log.get("file", "")
    iostat = json_log.get("iostat", "")
    volume = json_log.get("volume", "")
    host = json_log.get("host", "")
    if iostat != "":
        dev = iostat.get("Dev", "")
        tps = iostat.get("tps", "")
        BlkRead = iostat.get("Blk_read/s", "")
        BlkWrite = iostat.get("Blk_wrtn/s", "")
    csv_f.writerow([json_log["transferID"],
		json_log["event_type"],
		json_log["start_timestamp"],
		json_log["end_timestamp"],
		json_log["user"],
		filepath,
		json_log["tcp_bufsize"],
		json_log["globus_blocksize"],
		json_log["nbytes"],
		json_log["nstreams"],
		json_log["dest"],
		json_log["cmd_type"],
		json_log["ret_code"],
		json_log["mpstat"]["%usr"],
		json_log["mpstat"]["%nice"],
		json_log["mpstat"]["%sys"],
		json_log["mpstat"]["%iowait"],
		json_log["mpstat"]["%irq"],
		json_log["mpstat"]["%soft"],
		json_log["mpstat"]["%steal"],
		json_log["mpstat"]["%quest"],
		json_log["mpstat"]["%idle"],
		host,
		volume,
		dev,
		tps,
		BlkRead,
		BlkWrite,
		json_log["getrusage"]["ru_utime"],
		json_log["getrusage"]["ru_stime"],
		json_log["getrusage"]["ru_maxrss"],
		json_log["getrusage"]["ru_ixrss"],
		json_log["getrusage"]["ru_idrss"],
		json_log["getrusage"]["ru_isrss"],
		json_log["getrusage"]["ru_minflt"],
		json_log["getrusage"]["ru_majflt"],
		json_log["getrusage"]["ru_nswap"],
		json_log["getrusage"]["ru_inblock"],
		json_log["getrusage"]["ru_oublock"],
		json_log["getrusage"]["ru_msgsnd"],
#		json_log["getrusage"]["ru_msgrcv"],
		json_log["getrusage"]["ru_nsignals"],
		json_log["getrusage"]["ru_nvcsw"],
		json_log["getrusage"]["ru_nivcsw"],
		json_log["streams"][0]["TCPinfo"]["rcv_ssthresh"],
		json_log["streams"][0]["TCPinfo"]["rtt"],
		json_log["streams"][0]["TCPinfo"]["rttvar"],
		json_log["streams"][0]["TCPinfo"]["snd_ssthresh"],
		json_log["streams"][0]["TCPinfo"]["snd_cwnd"],
		json_log["streams"][0]["TCPinfo"]["advmss"],
		json_log["streams"][0]["TCPinfo"]["reordering"],
		json_log["streams"][0]["TCPinfo"]["rcv_rtt"],
		json_log["streams"][0]["TCPinfo"]["rcv_space"],
		json_log["streams"][0]["TCPinfo"]["total_retrans"]
               ])
f.close()
