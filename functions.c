void drefs_register(void)
{
int i;
    for(i=0;dt[i].dref_name[0];i++)
    {
      dt[i].ref = XPLMFindDataRef(dt[i].dref_name);
      if(dt[i].ref)
        log_write("DataRef found: %s",dt[i].dref_name);
    }
}

void dwrefs_register(void)
{
int i;
    for(i=0;dtw[i].dref_name[0];i++)
    {
      dtw[i].ref = XPLMFindDataRef(dtw[i].dref_name);
    }
}

void drefs_update(void)
{
int i;
    for(i=0;dt[i].dref_name[0];i++)
    {
       if(dt[i].ref)
       {
         switch(dt[i].var_type)
         {
           case xplmType_Int:
               dt[i].ival = XPLMGetDatai(dt[i].ref);

           break;

           case xplmType_Float:
               dt[i].fval = XPLMGetDataf(dt[i].ref);

           break;
        }
     }
    }

}


float drefs_getfloat_by_id(int id)
{
int i;
    for(i=0;dt[i].dref_name[0];i++)
    {
       if(dt[i].ref)
       {
         if(dt[i].id == id && dt[i].var_type == xplmType_Float)
         {

            return dt[i].fval;
         }

       }
    }
return 0;
}


int drefs_getint_by_id(int id)
{
int i;
    for(i=0;dt[i].dref_name[0];i++)
    {
       if(dt[i].ref)
       {
         if(dt[i].id == id && dt[i].var_type == xplmType_Int)
         {

            return dt[i].ival;
         }

       }
    }
return 0;
}

int drefs_setint_by_id(int id, int ival)
{
int i;
    for(i=0;dt[i].dref_name[0];i++)
    {
       if(dt[i].ref)
       {
         if(dt[i].id == id && dt[i].var_type == xplmType_Int)
         {
            XPLMSetDatai(dt[i].ref,ival);
         }

       }
    }
return 0;
}


int drefs_setfloat_by_id(int id, float fval)
{
int i;
    for(i=0;dt[i].dref_name[0];i++)
    {
       if(dt[i].ref)
       {
         if(dt[i].id == id && dt[i].var_type == xplmType_Float)
         {
            XPLMSetDataf(dt[i].ref,fval);
         }

       }
    }
return 0;
}

int dwrefs_setint_by_id(int id, int ival)
{
int i;
    for(i=0;dtw[i].dref_name[0];i++)
    {
       if(dtw[i].ref)
       {
         if(dtw[i].id == id && dtw[i].var_type == xplmType_Int)
         {
           XPLMSetDatai(dtw[i].ref,ival);
         }

       }
    }
return 0;
}



int dwrefs_setfloat_by_id(int id, float fval)
{
int i;
    for(i=0;dtw[i].dref_name[0];i++)
    {
       if(dtw[i].ref)
       {
         if(dtw[i].id == id && dtw[i].var_type == xplmType_Float)
         {
            XPLMSetDataf(dtw[i].ref,fval);
         }

       }
    }
return 0;
}

int dwrefs_setfloatarray_by_id(int id, float *fval,int offset, int count)
{
int i;
    for(i=0;dtw[i].dref_name[0];i++)
    {
       if(dtw[i].ref)
       {
         if(dtw[i].id == id && dtw[i].var_type == xplmType_FloatArray)
         {
            XPLMSetDatavf(dtw[i].ref,fval,offset,count);
         }

       }
    }
return 0;
}
