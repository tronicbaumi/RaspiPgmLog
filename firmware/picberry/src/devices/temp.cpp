bool Std33FGenericErase(DWORD EraseOpCode, DWORD Address)
{
    BYTE      QueueLoc = 0;
    bool      status = TRUE;
    // Setup EECON1
    Cmd33S[QueueLoc].command = cmd33FSIX;
    Cmd33S[QueueLoc].instruct = MoveLiteraltoWREG(EraseOpCode,REGW10); // 0x400E
    NewQueMember(QueueLoc);

    Cmd33S[QueueLoc].command = cmd33FSIX;
    Cmd33S[QueueLoc].instruct = opcode33FMOV_W10_EECON1;              //0x88468A
    NewQueMember(QueueLoc);

    // Start it and send
    Cmd33S[QueueLoc].command = cmd33FSIX;
    Cmd33S[QueueLoc].instruct = MoveLiteraltoWREG(NVM_UNLOCK_CODE1,REGW10);
    NewQueMember(QueueLoc);

    Cmd33S[QueueLoc].command = cmd33FSIX;
    Cmd33S[QueueLoc].instruct = opcode33FMOV_W10_NVMKEY;   // 0x8846BA
    NewQueMember(QueueLoc);

    Cmd33S[QueueLoc].command = cmd33FSIX;
    Cmd33S[QueueLoc].instruct = MoveLiteraltoWREG(NVM_UNLOCK_CODE2,REGW10);
    NewQueMember(QueueLoc);

    Cmd33S[QueueLoc].command = cmd33FSIX;
    Cmd33S[QueueLoc].instruct = opcode33FMOV_W10_NVMKEY;      // 0x8846BA
    NewQueMember(QueueLoc);

   Cmd33S[QueueLoc].command = cmd33FSIXWNOP | def33F_1_NOP;
   Cmd33S[QueueLoc].instruct = opcode33FBSET_EECON1_WR;         // 0xA8E8D1
   
   NewQueMember(QueueLoc);

    Send33CmdQueue(QueueLoc,Cmd33S);

   
    NOPS(3);
    Delayus(250000);

    status = WAITTILLDONE();
    return(status);
}
