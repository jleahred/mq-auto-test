BEGIN_INIT

INCLUDES = [ ]
NAMESPACES = ['ae', 'msg']

END_INIT



//---------------------------------------------------------------------
//        
//
//---------------------------------------------------------------------


sub_question [<  >]
{
    question            (q)     :   string
    [answers]           (a)     :   string
    [wrong_options]     (o)     :   string
}



sub_status [< >]
{
    started                 (s)     :   date_time
    repetitions             (r)     :   int
    penalizations           (p)     :   int
    options_per_question    (opq)   :   int
    failed                  (f)     :   int

    pendings                (p)     :   int
}
