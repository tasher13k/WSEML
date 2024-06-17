#include "../include/WSEML.h"
#include "../include/misc.h"
#include "../include/pointers.h"
#include "../include/dllconfig.h"
#include "../include/parser.h"
#include "../include/helpFunc.h"

/// [':=', dest:ref, data:ref, N:ps]bc
WSEML assignment(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &dataRef = cmdList->find(L"data");
    WSEML &destRef = cmdList->find(L"dest");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"assign_tmpPtr"));
    data->append(&data_o, *cmd, WSEML(L"assign_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"assign_tmp"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"assignment", L"16");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:assign_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:assign_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }


    changeCommand(stackList, equivKey, L"17");

    args->find(L"ref") = dataRef;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:assign_tmp]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"18");
    args->find(L"ref") = destRef;
    res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:assign_tmp, 2:assign_cmdCopy, 3:assign_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`+', R, O1, O2, N]bc
WSEML addition(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"add_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"add_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"add_O2"));
    data->append(&data_o, *cmd, WSEML(L"add_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"add_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"addition", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:add_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:add_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:add_O1]ps]ptr}");

    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:add_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:add_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:add_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:add_res]ps]ptr]ref}");
    safeSum(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:add_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:add_O1, 2:add_O2, 3:add_cmdCopy, 4:add_res, 5:add_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`-', R, O1, O2, N]bc
WSEML subtraction(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"sub_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"sub_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"sub_O2"));
    data->append(&data_o, *cmd, WSEML(L"sub_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"sub_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"subtraction", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:sub_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:sub_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }


    changeCommand(stackList, equivKey, L"20");
    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:sub_O1]ps]ptr}");

    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:sub_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:sub_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:sub_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:sub_res]ps]ptr]ref}");
    safeSub(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:sub_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:sub_O1, 2:sub_O2, 3:sub_cmdCopy, 4:sub_res, 5:sub_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`*', R, O1, O2, N]bc
WSEML multiplication(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"mult_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"mult_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"mult_O2"));
    data->append(&data_o, *cmd, WSEML(L"mult_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"mult_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"multiplication", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mult_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:mult_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }


    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mult_O1]ps]ptr}");

    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mult_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mult_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mult_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mult_res]ps]ptr]ref}");
    safeMult(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mult_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:mult_O1, 2:mult_O2, 3:mult_cmdCopy, 4:mult_res, 5:mult_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`/', R, O1, O2, N]bc
WSEML division(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"div_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"div_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"div_O2"));
    data->append(&data_o, *cmd, WSEML(L"div_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"div_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"division", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:div_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:div_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }


    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:div_O1]ps]ptr}");

    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:div_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:div_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:div_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:div_res]ps]ptr]ref}");
    safeDiv(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:div_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:div_O1, 2:div_O2, 3:div_cmdCopy, 4:div_res, 5:div_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`%', R, O1, O2, N]bc
WSEML remainder(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"mod_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"mod_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"mod_O2"));
    data->append(&data_o, *cmd, WSEML(L"mod_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"mod_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"remainder", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mod_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:mod_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mod_O1]ps]ptr}");

    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mod_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mod_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mod_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mod_res]ps]ptr]ref}");
    safeMod(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:mod_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:mod_O1, 2:mod_O2, 3:mod_cmdCopy, 4:mod_res, 5:mod_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`^', R, O1, O2, N]bc
WSEML power(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"pow_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"pow_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"pow_O2"));
    data->append(&data_o, *cmd, WSEML(L"pow_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"pow_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"power", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:pow_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:pow_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:pow_O1]ps]ptr}");

    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:pow_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:pow_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:pow_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:pow_res]ps]ptr]ref}");
    safePow(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:pow_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:pow_O1, 2:pow_O2, 3:pow_cmdCopy, 4:pow_res, 5:pow_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`.', R, O1, O2, N]bc
WSEML concatenate(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"concat_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"concat_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"concat_O2"));
    data->append(&data_o, *cmd, WSEML(L"concat_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"concat_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"concatenation", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:concat_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:concat_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:concat_O1]ps]ptr}");

    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:concat_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:concat_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:concat_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:concat_res]ps]ptr]ref}");
    safeConcat(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:concat_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:concat_O1, 2:concat_O2, 3:concat_cmdCopy, 4:concat_res, 5:concat_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘=’, R, O1, O2, N]bc
WSEML isEq(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"eq_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"eq_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"eq_O2"));
    data->append(&data_o, *cmd, WSEML(L"eq_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"eq_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"isEq", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:eq_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:eq_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:eq_O1]ps]ptr}");

    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:eq_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:eq_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:eq_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:eq_res]ps]ptr]ref}");
    safeEq(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:eq_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:eq_O1, 2:eq_O2, 3:eq_cmdCopy, 4:eq_res, 5:eq_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘!=’, R, O1, O2, N]bc
WSEML isNeq(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"neq_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"neq_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"neq_O2"));
    data->append(&data_o, *cmd, WSEML(L"neq_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"neq_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"isNeq", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:neq_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:neq_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:neq_O1]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:neq_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:neq_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:neq_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:neq_res]ps]ptr]ref}");
    safeNeq(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:neq_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:neq_O1, 2:neq_O2, 3:neq_cmdCopy, 4:neq_res, 5:neq_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘<', R, O1, O2, N]bc
WSEML isLess(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"less_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"less_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"less_O2"));
    data->append(&data_o, *cmd, WSEML(L"less_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"less_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"isLess", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:less_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:less_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:less_O1]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:less_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:less_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:less_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:less_res]ps]ptr]ref}");
    safeLess(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:less_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:less_O1, 2:less_O2, 3:less_cmdCopy, 4:less_res, 5:less_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘>’, R, O1, O2, N]bc
WSEML isGreater(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"greater_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"greater_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"greater_O2"));
    data->append(&data_o, *cmd, WSEML(L"greater_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"greater_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"isGreater", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:greater_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:greater_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:greater_O1]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:greater_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:greater_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:greater_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:greater_res]ps]ptr]ref}");
    safeGreater(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:greater_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:greater_O1, 2:greater_O2, 3:greater_cmdCopy, 4:greater_res, 5:greater_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘<=’, R, O1, O2, N]bc
WSEML isLeq(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"leq_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"leq_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"leq_O2"));
    data->append(&data_o, *cmd, WSEML(L"leq_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"leq_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"isLeq", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:leq_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:leq_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:leq_O1]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:leq_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:leq_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:leq_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:leq_res]ps]ptr]ref}");
    safeLeq(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:leq_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:leq_O1, 2:leq_O2, 3:leq_cmdCopy, 4:leq_res, 5:leq_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘>=’, R, O1, O2, N]bc
WSEML isGeq(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"geq_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"geq_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"geq_O2"));
    data->append(&data_o, *cmd, WSEML(L"geq_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"geq_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"isGeq", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:geq_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:geq_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:geq_O1]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:geq_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:geq_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:geq_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:geq_res]ps]ptr]ref}");
    safeGeq(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:geq_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:geq_O1, 2:geq_O2, 3:geq_cmdCopy, 4:geq_res, 5:geq_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`&&', R, O1, O2, N]bc
WSEML logicAnd(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"and_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"and_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"and_O2"));
    data->append(&data_o, *cmd, WSEML(L"and_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"and_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"logicAnd", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:and_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:and_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:and_O1]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:and_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:and_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:and_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:and_res]ps]ptr]ref}");
    safeAnd(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:and_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:and_O1, 2:and_O2, 3:and_cmdCopy, 4:and_res, 5:and_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`||', R, O1, O2, N]bc
WSEML logicOr(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O1 = cmdList->find(L"O1");
    WSEML &O2 = cmdList->find(L"O2");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"or_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"or_O1"));
    data->append(&data_o, NULLOBJ, WSEML(L"or_O2"));
    data->append(&data_o, *cmd, WSEML(L"or_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"or_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"logicOr", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:or_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:or_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = O1;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:or_O1]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = O2;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:or_O2]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O1:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:or_O1]ps]ptr]ref, O2:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:or_O2]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:or_res]ps]ptr]ref}");
    safeOr(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:or_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:or_O1, 2:or_O2, 3:or_cmdCopy, 4:or_res, 5:or_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`!', R, O, N]bc
WSEML logicNot(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &O = cmdList->find(L"O");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"not_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"not_O"));
    data->append(&data_o, *cmd, WSEML(L"not_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"not_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"logicNot", L"18");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:not_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:not_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"19");

    args->find(L"ref") = O;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:not_O]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML OpArgs = parse(L"{O:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:not_O]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:not_res]ps]ptr]ref}");
    safeNot(OpArgs);

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:not_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:not_O, 2:not_cmdCopy, 3:not_res, 4:not_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘I’, R, L, RK, K, RD, D, I, N]bc
/// K is put in the R (if K=$, then generated one)
WSEML insert(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &L = cmdList->find(L"L");
    WSEML &RK = cmdList->find(L"RK");
    WSEML &K = cmdList->find(L"K");
    WSEML &RD = cmdList->find(L"RD");
    WSEML &D = cmdList->find(L"D");
    WSEML &I = cmdList->find(L"I");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"insert_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"insert_L"));
    data->append(&data_o, NULLOBJ, WSEML(L"insert_RK"));
    data->append(&data_o, NULLOBJ, WSEML(L"insert_K"));
    data->append(&data_o, NULLOBJ, WSEML(L"insert_RD"));
    data->append(&data_o, NULLOBJ, WSEML(L"insert_D"));
    data->append(&data_o, NULLOBJ, WSEML(L"insert_I"));
    data->append(&data_o, *cmd, WSEML(L"insert_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"insert_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"insert", L"23");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:insert_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"24");
    args->find(L"ref") = L;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_L]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"25");
    args->find(L"ref") = RK;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_RK]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"26");
    args->find(L"ref") = K;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_K]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"27");
    args->find(L"ref") = RD;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_RD]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"28");
    args->find(L"ref") = D;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_D]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"29");
    args->find(L"ref") = I;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_I]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML OpArgs = parse(
            L"{L:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_L]ps]ptr]ref, L"
                                                         "RK:$[type:d, 1:$[comp:$[addr:" + procStr +
            L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_RK]ps]ptr]ref, L"
            "K:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_K]ps]ptr]ref, L"
                                                       "RD:$[type:d, 1:$[comp:$[addr:" + procStr +
            L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_RD]ps]ptr]ref, L"
            "D:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_D]ps]ptr]ref, L"
                                                       "I:$[type:d, 1:$[comp:$[addr:" + procStr +
            L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_I]ps]ptr]ref, L"
            "res:$[type:d, 1:$[comp:$[addr:" + procStr +
            L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_res]ps]ptr]ref}");
    safeInsert(OpArgs);


    changeCommand(stackList, equivKey, L"31");
    args->find(L"ref") = L;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_L]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"32");
    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:insert_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(
            L"{1:insert_tmpPtr, 2:insert_cmdCopy, 3:insert_L, 4:insert_RK, 5:insert_K, 6:insert_RD, 7:insert_D, 8:insert_I, 9:insert_res}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘D’, O, N]bc
WSEML erase(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &O = cmdList->find(L"O");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"erase_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"erase_O"));
    data->append(&data_o, *cmd, WSEML(L"erase_cmdCopy"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"erase", L"17");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:erase_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:erase_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"18");

    args->find(L"ref") = O;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:erase_O]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML OpArgs = parse(
            L"{O:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:erase_O]ps]ptr]ref}");
    safeErase(OpArgs);

    WSEML DataKeys = parse(L"{1:erase_O, 2:erase_cmdCopy, 3:erase_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘E’, R, O, N]bc
WSEML isDeref(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &O = cmdList->find(L"O");
    WSEML &R = cmdList->find(L"R");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"isDeref_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"isDeref_O"));
    data->append(&data_o, NULLOBJ, WSEML(L"isDeref_res"));
    data->append(&data_o, *cmd, WSEML(L"isDeref_cmdCopy"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"isDeref", L"17");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:isDeref_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:isDeref_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));
    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"18");

    args->find(L"ref") = O;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:isDeref_O]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(
            L"{O:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:isDeref_O]ps]ptr]ref, L"
                                                         "res:$[type:d, 1:$[comp:$[addr:" + procStr +
            L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:isDeref_res]ps]ptr]ref}");
    safeIsDeref(OpArgs);

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:isDeref_res]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML DataKeys = parse(L"{1:isDeref_O, 2:isDeref_res, 3:isDeref_cmdCopy, 4:isDeref_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// ['C', R, F, A, N]bc
WSEML call(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &F = cmdList->find(L"F");
    WSEML &A = cmdList->find(L"A");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"call_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"call_F"));
    data->append(&data_o, NULLOBJ, WSEML(L"call_A"));
    data->append(&data_o, *cmd, WSEML(L"call_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"call_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"call", L"19");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:call_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:call_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"20");

    args->find(L"ref") = F;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:call_F]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = A;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:call_A]ps]ptr}");
    res = callFunc(readDll.c_str(), readDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{F:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:call_F]ps]ptr]ref, A:$[type:d, 1:$[comp:$[addr:" +
                         procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:call_A]ps]ptr]ref, res:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:call_res]ps]ptr]ref}");
    safeCall(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:call_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:call_F, 2:call_A, 3:call_cmdCopy, 4:call_res, 5:call_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘P’, O, N]bc
WSEML lastToI(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &O = cmdList->find(L"O");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"lastToI_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"lastToI_O"));
    data->append(&data_o, *cmd, WSEML(L"lastToI_cmdCopy"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"lastToI", L"17");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:lastToI_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:lastToI_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"18");

    args->find(L"ref") = O;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:lastToI_O]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML OpArgs = parse(
            L"{O:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:lastToI_O]ps]ptr]ref}");
    safeToI(OpArgs);

    WSEML DataKeys = parse(L"{1:lastToI_O, 2:lastToI_cmdCopy, 3:lastToI_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [‘K’, O, N]bc
WSEML lastToK(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &O = cmdList->find(L"O");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"lastToK_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"lastToK_O"));
    data->append(&data_o, *cmd, WSEML(L"lastToK_cmdCopy"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"lastToI", L"17");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:lastToK_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:lastToK_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"18");

    args->find(L"ref") = O;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:lastToK_O]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML OpArgs = parse(
            L"{O:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:lastToK_O]ps]ptr]ref}");
    safeToK(OpArgs);

    WSEML DataKeys = parse(L"{1:lastToK_O, 2:lastToK_cmdCopy, 3:lastToK_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`U', R, D, N]
WSEML callPrevDisp(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &D = cmdList->find(L"D");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"callPrevDisp_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"callPrevDisp_D"));
    data->append(&data_o, *cmd, WSEML(L"callPrevDisp_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"callPrevDisp_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"callPrevDisp", L"18");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevDisp_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:callPrevDisp_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"19");

    args->find(L"ref") = D;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevDisp_D]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML OpArgs = parse(L"{stck:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:stck]ps]ptr]ref, L"
                                                                         "stack:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:stck]ps, 2:$[t:k, k:" + curStackKey + L"]ps]ptr]ref, L"
                                                                                  "D:$[type:d, 1:$[comp:$[addr:" +
                         procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevDisp_D]ps]ptr]ref, L"
                                   "res:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevDisp_res]ps]ptr]ref}");
    safeCallPrevDisp(OpArgs);

    changeCommand(stackList, equivKey, L"23");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevDisp_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:callPrevDisp_D, 2:callPrevDisp_cmdCopy, 3:callPrevDisp_res, 4:callPrevDisp_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`V', R, D, N]
WSEML callPrevProg(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &R = cmdList->find(L"R");
    WSEML &D = cmdList->find(L"D");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"callPrevProg_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"callPrevProg_D"));
    data->append(&data_o, *cmd, WSEML(L"callPrevProg_cmdCopy"));
    data->append(&data_o, NULLOBJ, WSEML(L"callPrevProg_res"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"callPrevProg", L"21");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevProg_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:callPrevProg_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));

    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"22");

    args->find(L"ref") = D;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevProg_D]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML OpArgs = parse(
            L"{stack:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:stck]ps, 2:$[t:k, k:" + curStackKey +
            L"]ps]ptr]ref, L"
            "frm:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:stck]ps, 2:$[t:k, k:" + curStackKey +
            L"]ps, 3:$[t:k, k:" + curFrmKey + L"]ps]ptr]ref, L"
                                              "D:$[type:d, 1:$[comp:$[addr:" + procStr +
            L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevProg_D]ps]ptr]ref, L"
            "res:$[type:d, 1:$[comp:$[addr:" + procStr +
            L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevProg_res]ps]ptr]ref}");
    safeCallPrevProg(OpArgs);

    changeCommand(stackList, equivKey, L"27");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:callPrevProg_res]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeDll.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:callPrevProg_D, 2:callPrevProg_cmdCopy, 3:callPrevProg_res, 4:callPrevProg_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`T', R, O, N]bc
WSEML readType(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &O = cmdList->find(L"O");
    WSEML &R = cmdList->find(L"R");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"readType_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"readType_O"));
    data->append(&data_o, NULLOBJ, WSEML(L"readType_res"));
    data->append(&data_o, *cmd, WSEML(L"readType_cmdCopy"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"readType", L"18");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:readType_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:readType_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));
    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"19");

    args->find(L"ref") = O;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:readType_O]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML OpArgs = parse(L"{O:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:readType_O]ps]ptr]ref, L"
                         "res:$[type:d, 1:$[comp:$[addr:" + procStr +
                         L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:readType_res]ps]ptr]ref}");
    safeReadType(OpArgs);

    changeCommand(stackList, equivKey, L"21");

    args->find(L"ref") = R;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:readType_res]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;


    WSEML DataKeys = parse(L"{1:readType_O, 2:readType_res, 3:readType_cmdCopy, 4:readType_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}

/// [`S', O, T, N]bc
WSEML setType(const WSEML &Args) {
    List *ArgsList = dynamic_cast<List *>(Args.getObj());
    WSEML *obj = extractObj(ArgsList->find(L"obj"));
    WSEML *stack = extractObj(ArgsList->find(L"stack"));
    WSEML *frm = extractObj(ArgsList->find(L"frm"));
    WSEML *cmd = extractObj(ArgsList->find(L"cmd"));

    List *cmdList = dynamic_cast<List *>(cmd->getObj());
    WSEML &O = cmdList->find(L"O");
    WSEML &T = cmdList->find(L"T");

    List *proc = dynamic_cast<List *>(obj->getObj());
    List *tables = dynamic_cast<List *>(proc->find(L"tables").getObj());
    WSEML &data_o = proc->find(L"data");
    List *data = dynamic_cast<List *>(data_o.getObj());
    List *uref = dynamic_cast<List *>(tables->find(L"uref").getObj());
    List *readList = dynamic_cast<List *>(uref->find(L"read").getObj());
    List *writeList = dynamic_cast<List *>(uref->find(L"write").getObj());
    List *stackList = dynamic_cast<List *>(stack->getObj());
    List *stackInfo = dynamic_cast<List *>(stackList->find(L"info").getObj());
    WSEML *wfrm = &stackInfo->find(L"wfrm");

    std::wstring readDll = dynamic_cast<ByteString *>(readList->find(L"dllName").getObj())->get();
    std::wstring readFunc = dynamic_cast<ByteString *>(readList->find(L"funcName").getObj())->get();
    std::wstring writeDll = dynamic_cast<ByteString *>(writeList->find(L"dllName").getObj())->get();
    std::wstring writeFunc = dynamic_cast<ByteString *>(writeList->find(L"funcName").getObj())->get();

    std::wstring curStackKey = dynamic_cast<ByteString *>(stack->getPair()->getKey().getObj())->get();
    std::wstring curFrmKey = dynamic_cast<ByteString *>(frm->getPair()->getKey().getObj())->get();
    data->append(&data_o,
                 parse(L"{1:$[t:r]ps, 2:$[t:k, k:stck]ps, 3:$[t:k, k:" + curStackKey + L"]ps, 4:$[t:k, k:" + curFrmKey +
                       L"]ps, 5:$[t:k, k:ip]ps, 6:$[t:i, i:-1]ps}"), WSEML(L"setType_tmpPtr"));
    data->append(&data_o, NULLOBJ, WSEML(L"setType_O"));
    data->append(&data_o, NULLOBJ, WSEML(L"setType_T"));
    data->append(&data_o, *cmd, WSEML(L"setType_cmdCopy"));

    WSEML equivKey = createEquiv(stack, wfrm, frm, L"setType", L"18");

    std::wstring procStr = getAddrStr(obj);
    WSEML ipRef = parse(L"{type:d, 1:$[type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:setType_tmpPtr]ps]ptr]ref}");
    WSEML newPs = parse(L"{type:d, 1:$[comp:$[addr:" + procStr +
                        L"]ptr, 2:$[t:k, k:data]ps, 3:$[t:k, k:setType_cmdCopy]ps, 4$[t:k, k:N]ps]ptr}");
    ipRef.setType(WSEML(L"ref"));
    newPs.setType(WSEML(L"ref"));
    std::list<Pair> tmp;
    WSEML refArgs(tmp);
    List *args = dynamic_cast<List *>(refArgs.getObj());
    args->append(&refArgs, ipRef, WSEML(L"ref"));
    args->append(&refArgs, newPs, WSEML(L"data"));
    WSEML res;
    if (cmdList->back() != NULLOBJ) {
        res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
        if (res == WSEML(L"stopped")) return res;
    }

    changeCommand(stackList, equivKey, L"19");

    args->find(L"ref") = O;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:setType_O]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    changeCommand(stackList, equivKey, L"20");
    args->find(L"ref") = T;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:setType_T]ps]ptr}");
    res = callFunc(readDll.c_str(), readFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML OpArgs = parse(
            L"{O:$[type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:setType_O]ps]ptr]ref, L"
                                                         "T:$[type:d, 1:$[comp:$[addr:" + procStr +
            L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:setType_T]ps]ptr]ref}");
    safeSetType(OpArgs);

    changeCommand(stackList, equivKey, L"22");
    args->find(L"ref") = O;
    args->find(L"data") = parse(
            L"{type:d, 1:$[comp:$[addr:" + procStr + L"]ptr, 1:$[t:k, k:data]ps, 2:$[t:k, k:setType_O]ps]ptr}");
    res = callFunc(writeDll.c_str(), writeFunc.c_str(), refArgs);
    if (res == WSEML(L"stopped")) return res;

    WSEML DataKeys = parse(L"{1:setType_O, 2:setType_T, 3:setType_cmdCopy, 4:setType_tmpPtr}");
    clear(stackList, data, wfrm, equivKey, DataKeys);

    return {L"completed"};
}