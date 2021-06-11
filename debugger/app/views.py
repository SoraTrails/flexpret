import os
import collections
import bisect
from django.template import loader

# Create your views here.
from django.http import HttpResponse
from django.views.decorators.clickjacking import xframe_options_exempt

insts = []
confs = []
asm_dict = {}
asm_func = collections.OrderedDict()

def download():
    global insts, confs, asm_dict, asm_func
    code = os.system('scp -P 2238 zhaoxiujia@xxx:~/flexpret_dev/flexpret/emulator/generated-src/8tf-64i-64d-ti/log.tar.gz ./log.tar.gz')
    if code:
        print('scp failed')
        return [], []
    os.system("gunzip -c ./log.tar.gz > ./log")
    code = os.system('scp -P 2238 zhaoxiujia@xxx:~/flexpret_dev/flexpret/emulator/generated-src/8tf-64i-64d-ti/config ./config')
    if code:
        print('scp failed')
        return [], []
    code = os.system('scp -P 2238 zhaoxiujia@xxx:~/flexpret_dev/flexpret/emulator/generated-src/8tf-64i-64d-ti/asm ./asm')
    if code:
        print('scp failed')
        return [], []
    code = os.system('scp -P 2238 zhaoxiujia@xxx:~/flexpret_dev/flexpret/emulator/generated-src/8tf-64i-64d-ti/asm_func ./asm_func')
    if code:
        print('scp failed')
        return [], []

    insts = []
    confs = []
    try:
        with open('./config', 'r') as config_file:
            conf = config_file.readline().strip()
            conf = '' if not conf else conf.split()[1]
            while conf:
                confs.append(conf)
                conf = config_file.readline().strip()
                conf = '' if not conf else conf.split()[1]
    except Exception as e:
        print(len(confs))
        print(e)
        
    with open('./log', 'r') as log_file:
        inst = log_file.readline().strip()
        cycle = 0
        while inst:
            args = inst.split()
            if len(args) != len(confs):
                break
            i = {'cycle': cycle}
            for a, c in zip(args, confs):
                i[c] = a
            insts.append(i)
            inst = log_file.readline().strip()
            cycle += 1

    asm_dict = {}
    with open('./asm', 'r') as asm_file:
        asm = asm_file.readline().strip()
        while asm:
            args = asm.split()
            asm_dict[args[0]] = ' '.join(args[1:])
            asm = asm_file.readline().strip()
    
    asm_func = collections.OrderedDict()
    with open('./asm_func', 'r') as asm_file:
        asm = asm_file.readline().strip()
        while asm:
            args = asm.split()
            pc = int(args[0], 16)
            asm_func[pc] = args[1]
            asm = asm_file.readline().strip()

def get_func(pc):
    global asm_func
    asm_keys = list(asm_func.keys())
    if pc in asm_keys:
        return asm_func.get(pc, '')
    index = bisect.bisect_left(asm_keys, pc, lo=0, hi=len(asm_keys))
    if index == 0:
        index = 1
    return asm_func.get(asm_keys[index - 1], '')

CYCLE = 'cycle'
INST = 'Core_datapath__exe_reg_inst'
PC = 'Core_datapath__exe_reg_pc'
TID = 'Core_datapath__exe_reg_tid'
VALID = 'Core_control__exe_valid'
OP1 = 'Core_datapath__exe_reg_op1'
OP2 = 'Core_datapath__exe_reg_op2'

def index(request):
    global insts, confs, asm_dict
    template = loader.get_template('app/index.html')
    if not insts or not confs or int(request.GET.get('refresh', 0) or 0):
        download()
    page = int(request.GET.get('page', 1) or 1)
    if len(insts) == 0:
        return HttpResponse(content=b'insts is empty', status=500)
    page_len = 50
    page_max = int((len(insts) - 1) / page_len) + 1
    if page == -1:
        page = page_max
    if page < 1 or page > (len(insts) - 1) / page_len + 1:
        return HttpResponse(content=b'bad page number', status=400)
    begin = int((page - 1) * page_len)
    end = int(page * page_len)
    paged_insts = insts[begin:end]
    context = {
        # 'head' : ['cycle', 'pc', 'tid', 'valid', 'inst', 'asm', 'op1', 'op2'],
        'head' : ['cycle', 'pc', 'tid', 'valid', 'inst', 'asm', 'func'],
        # 'instruction_list': map(lambda x: {'cycle': x[CYCLE], 'pc': x[PC], 'tid': int(x[TID]), 'valid': int(x[VALID]), 'inst': x[INST], 'asm': asm_dict.get(hex(int(x[PC], 16))[2:], ''), 'op1': x[OP1], 'op2': x[OP2]}, paged_insts),
        'instruction_list': map(lambda x: {'cycle': x[CYCLE], 'pc': x[PC], 'tid': int(x[TID]), 'valid': int(x[VALID]), 'inst': x[INST], 'asm': asm_dict.get(hex(int(x[PC], 16))[2:], ''), 'func': get_func(int(x[PC], 16))}, paged_insts),
        'page_1': page - 1 if page > 1 else 1,
        'page_2': page + 1 if page < page_max else page_max,
        'page_3': page - 5 if page > 5 else 1,
        'page_4': page + 5 if page < page_max - 5 else page_max,

        'page_max': page_max
    }
    return HttpResponse(template.render(context, request))

@xframe_options_exempt
def instrucion(request, cycle):
    global insts
    template = loader.get_template('app/inst.html')
    if not insts or len(insts) <= int(cycle):
        download()
    context = {
        'detail_inst': collections.OrderedDict(sorted(insts[int(cycle)].items(), reverse=True))
    }
    return HttpResponse(template.render(context, request))

def thread(request):
    global insts, confs, asm_dict
    template = loader.get_template('app/thread.html')
    if not insts or not confs or int(request.GET.get('refresh', 0) or 0):
        download()
    page = int(request.GET.get('page', 1) or 1)
    if len(insts) == 0:
        return HttpResponse(content=b'insts is empty', status=500)
    page_len = 40
    page_max = int((len(insts) - 1) / page_len) + 1
    if page == -1:
        page = page_max
    if page < 1 or page > (len(insts) - 1) / page_len + 1:
        return HttpResponse(content=b'bad page number', status=400)
    begin = int((page - 1) * page_len)
    end = int(page * page_len)
    paged_insts = insts[begin:end]

    thread_insts = [ [x if int(x[TID]) == i else None for x in paged_insts] for i in range(4) ]
    # thread_insts = [ list(filter(lambda x: int(x[TID]) == i, paged_insts)) for i in range(3) ]
    # max_len = max(map(lambda x: len(x), thread_insts))
    # page_len = 50
    # page_max = int((max_len - 1) / page_len) + 1
    # if page == -1:
    #     page = page_max
    # if page < 1 or page > (max_len - 1) / page_len + 1:
    #     return HttpResponse(content=b'bad page number', status=400)
    # begin = int((page - 1) * page_len)
    # end = int(page * page_len)
    # paged_insts = map(lambda x: x[begin:end], thread_insts)
    context = {
        # 'head' : ['cycle', 'pc', 'tid', 'valid', 'inst', 'asm', 'op1', 'op2'],
        'head' : ['cycle', 'pc', 'tid', 'valid', 'inst', 'func', 'asm'],
        # 'instruction_list': map(lambda x: {'cycle': x[CYCLE], 'pc': x[PC], 'tid': int(x[TID]), 'valid': int(x[VALID]), 'inst': x[INST], 'asm': asm_dict.get(hex(int(x[PC], 16))[2:], ''), 'op1': x[OP1], 'op2': x[OP2]}, paged_insts),
        'instruction_list': [
                map(lambda x: {
                        'cycle': x[CYCLE], 'pc': x[PC], 'tid': int(x[TID]), 'valid': int(x[VALID]), 'inst': x[INST], 
                        'func': get_func(int(x[PC], 16)), 'asm': asm_dict.get(hex(int(x[PC], 16))[2:], '')
                    } if x else None, y)
            for y in thread_insts
        ],
        'page_1': page - 1 if page > 1 else 1,
        'page_2': page + 1 if page < page_max else page_max,
        'page_3': page - 5 if page > 5 else 1,
        'page_4': page + 5 if page < page_max - 5 else page_max,

        'page_max': page_max
    }
    return HttpResponse(template.render(context, request))
