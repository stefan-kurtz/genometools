#!/usr/bin/env ruby

keylist = ["PLAINSEQ","ENCSEQ","INTSEQ"]

def getc_param(key)
  if key == "PLAINSEQ"
    return "const GtUchar *plainseq"
  elsif key == "INTSEQ"
    return "const GtUsainindextype *array"
  else
    return "const GtEncseq *encseq"
  end
end

def getc_call(key,posexpr)
  if key == "PLAINSEQ"
    return "(GtUword)\nplainseq[#{posexpr}]"
  elsif key == "INTSEQ"
    return "(GtUword) array[#{posexpr}]"
  else
    return "ISSPECIAL(tmpcc = gt_encseq_get_encoded_char(\n" +
                                  "encseq,\n" +
                                  "(GtUword) (#{posexpr}),\n" +
                                  "sainseq->readmode))\n" +
               "  ? GT_UNIQUEINT(#{posexpr}) : (GtUword) tmpcc"
  end
end

def declare_tmpcc(key,context)
  if key == "ENCSEQ"
    return "GtUchar tmpcc;\n#{context};"
  else
    return "#{context};"
  end
end

def sstarfirstcharcount_bucketsize_assert_not_NULL(key,which)
  if which.member?(key)
    return "gt_assert(bucketsize != NULL && sstarfirstcharcount != NULL);"
  else
    return ""
  end
end


def sstarfirstcharcount_update(key,struct,which)
  if which.member?(key)
    if struct
      return "sainseq->sstarfirstcharcount[nextcc]++;"
    else
      return "sstarfirstcharcount[nextcc]++;"
    end
  else
    return ""
  end
end

def bucketsize_update(key,which)
  if which.member?(key)
    return "bucketsize[currentcc]++;"
  else
    return ""
  end
end

begin
  fo = File.open("src/match/sfx-sain.inc","w")
rescue => err
  STDERR.puts "#{$0}: #{err}"
  exit 1
end

keylist.each do |key|
fo.puts <<CCODE
static GtUword gt_sain_#{key}_insertSstarsuffixes(GtSainseq *sainseq,
                                                 #{getc_param(key)},
                                                 GtUsainindextype *suftab,
                                                 GtLogger *logger)
{
  GtUword nextcc = GT_UNIQUEINT(sainseq->totallength),
          countSstartype = 0;
  GtUsainindextype position, *fillptr = sainseq->bucketfillptr;
  GtSainbuffer *sainbuffer = gt_sainbuffer_new(suftab,fillptr,
                                               sainseq->numofchars,logger);
  #{declare_tmpcc(key,"bool nextisStype = true")}

  gt_sain_endbuckets(sainseq);
  for (position = (GtUsainindextype) (sainseq->totallength-1); /* Nothing */;
       position--)
  {
    GtUword currentcc = #{getc_call(key,"position")};
    bool currentisStype = (currentcc < nextcc ||
                           (currentcc == nextcc && nextisStype)) ? true : false;
    if (!currentisStype && nextisStype)
    {
      countSstartype++;
#{sstarfirstcharcount_update(key,true,["PLAINSEQ","ENCSEQ"])}
      if (sainbuffer != NULL)
      {
        gt_sainbuffer_update(sainbuffer,nextcc,position);
      } else
      {
        suftab[--fillptr[nextcc]] = position;
      }
#undef SAINSHOWSTATE
#ifdef SAINSHOWSTATE
      printf("Sstar.suftab[" GT_WU "]=" GT_WU "\\n",fillptr[nextcc],position+1);
#endif
    }
    nextisStype = currentisStype;
    nextcc = currentcc;
    if (position == 0)
    {
      break;
    }
  }
  gt_sainbuffer_flushall(sainbuffer);
  gt_sainbuffer_delete(sainbuffer);
  gt_assert(GT_MULT2(countSstartype) <= sainseq->totallength);
  return countSstartype;
}

static void gt_sain_#{key}_fast_induceLtypesuffixes1(GtSainseq *sainseq,
                                                 #{getc_param(key)},
                                         GtSsainindextype *suftab,
                                         GtUword nonspecialentries)
{
  GtUword lastupdatecc = 0;
  GtSsainindextype *suftabptr, *bucketptr = NULL;
  GtUsainindextype *fillptr = sainseq->bucketfillptr;
  #{declare_tmpcc(key,"GtSsainindextype position")}

  gt_assert(sainseq->roundtable != NULL);
  for (suftabptr = suftab, sainseq->currentround = 0;
       suftabptr < suftab + nonspecialentries; suftabptr++)
  {
    if ((position = *suftabptr) > 0)
    {
      GtUword currentcc;

      if (position >= (GtSsainindextype) sainseq->totallength)
      {
        sainseq->currentround++;
        position -= (GtSsainindextype) sainseq->totallength;
      }
      currentcc = #{getc_call(key,"position")};
      if (currentcc < sainseq->numofchars)
      {
        if (position > 0)
        {
          GtUword t, leftcontextcc;

          position--;
          leftcontextcc = #{getc_call(key,"position")};
          t = (currentcc << 1) | (leftcontextcc < currentcc ? 1UL : 0);
          gt_assert(currentcc > 0 &&
                    sainseq->roundtable[t] <= sainseq->currentround);
          if (sainseq->roundtable[t] < sainseq->currentround)
          {
            position += (GtSsainindextype) sainseq->totallength;
            sainseq->roundtable[t] = sainseq->currentround;
          }
          GT_SAINUPDATEBUCKETPTR(currentcc);
          /* negative => position does not derive L-suffix
             positive => position may derive L-suffix */
          gt_assert(suftabptr < bucketptr);
          *bucketptr++ = (t & 1UL) ? ~position : position;
          *suftabptr = 0;
#ifdef SAINSHOWSTATE
          gt_assert(bucketptr != NULL);
          printf("L-induce: suftab[" GT_WU "]=" GT_WD "\\n",
                  (GtUword) (bucketptr-1-suftab),*(bucketptr-1));
#endif
        }
      } else
      {
        *suftabptr = 0;
      }
    } else
    {
      if (position < 0)
      {
        *suftabptr = ~position;
      }
    }
  }
}

static void gt_sain_#{key}_induceLtypesuffixes1(GtSainseq *sainseq,
                                                 #{getc_param(key)},
                                         GtSsainindextype *suftab,
                                         GtUword nonspecialentries)
{
  GtUword lastupdatecc = 0;
  GtUsainindextype *fillptr = sainseq->bucketfillptr;
  GtSsainindextype *suftabptr, *bucketptr = NULL;
  #{declare_tmpcc(key,"GtSsainindextype position")}

  gt_assert(sainseq->roundtable == NULL);
  for (suftabptr = suftab; suftabptr < suftab + nonspecialentries; suftabptr++)
  {
    if ((position = *suftabptr) > 0)
    {
      GtUword currentcc = #{getc_call(key,"position")};
      if (currentcc < sainseq->numofchars)
      {
        if (position > 0)
        {
          GtUword leftcontextcc;

          position--;
          leftcontextcc = #{getc_call(key,"position")};
          GT_SAINUPDATEBUCKETPTR(currentcc);
          /* negative => position does not derive L-suffix
             positive => position may derive L-suffix */
          gt_assert(suftabptr < bucketptr);
          *bucketptr++ = (leftcontextcc < currentcc) ? ~position : position;
          *suftabptr = 0;
#ifdef SAINSHOWSTATE
          gt_assert(bucketptr != NULL);
          printf("L-induce: suftab[" GT_WU "]=" GT_WD "\\n",
                  (GtUword) (bucketptr-1-suftab),*(bucketptr-1));
#endif
        }
      } else
      {
        *suftabptr = 0;
      }
    } else
    {
      if (position < 0)
      {
        *suftabptr = ~position;
      }
    }
  }
}

static void gt_sain_#{key}_fast_induceStypesuffixes1(GtSainseq *sainseq,
                                                 #{getc_param(key)},
                                         GtSsainindextype *suftab,
                                         GtUword nonspecialentries)
{
  GtUword lastupdatecc = 0;
  GtUsainindextype *fillptr = sainseq->bucketfillptr;
  GtSsainindextype *suftabptr, *bucketptr = NULL;
  #{declare_tmpcc(key,"GtSsainindextype position")}

  gt_assert(sainseq->roundtable != NULL);
  gt_sain_special_singleSinduction1(sainseq,
                                    suftab,
                                    (GtSsainindextype)
                                    (sainseq->totallength-1));
  if (sainseq->seqtype == GT_SAIN_ENCSEQ)
  {
    gt_sain_induceStypes1fromspecialranges(sainseq,
                                           sainseq->seq.encseq,
                                           suftab);
  }
  for (suftabptr = suftab + nonspecialentries - 1; suftabptr >= suftab;
       suftabptr--)
  {
    if ((position = *suftabptr) > 0)
    {
      if (position >= (GtSsainindextype) sainseq->totallength)
      {
        sainseq->currentround++;
        position -= (GtSsainindextype) sainseq->totallength;
      }
      if (position > 0)
      {
        GtUword currentcc = #{getc_call(key,"position")};

        if (currentcc < sainseq->numofchars)
        {
          GtUword t, leftcontextcc;

          position--;
          leftcontextcc = #{getc_call(key,"position")};
          t = (currentcc << 1) | (leftcontextcc > currentcc ? 1UL : 0);
          gt_assert(sainseq->roundtable[t] <= sainseq->currentround);
          if (sainseq->roundtable[t] < sainseq->currentround)
          {
            position += (GtSsainindextype) sainseq->totallength;
            sainseq->roundtable[t] = sainseq->currentround;
          }
          GT_SAINUPDATEBUCKETPTR(currentcc);
          gt_assert(bucketptr != NULL && bucketptr - 1 < suftabptr);
          *(--bucketptr) = (t & 1UL) ? ~(position+1) : position;
#ifdef SAINSHOWSTATE
          printf("S-induce: suftab[" GT_WU "]=" GT_WD "\\n",
                  (GtUword) (bucketptr - suftab),*bucketptr);
#endif
        }
      }
      *suftabptr = 0;
    }
  }
}

static void gt_sain_#{key}_induceStypesuffixes1(GtSainseq *sainseq,
                                                 #{getc_param(key)},
                                         GtSsainindextype *suftab,
                                         GtUword nonspecialentries)
{
  GtUword lastupdatecc = 0;
  GtUsainindextype *fillptr = sainseq->bucketfillptr;
  GtSsainindextype *suftabptr, *bucketptr = NULL;
  #{declare_tmpcc(key,"GtSsainindextype position")}

  gt_assert(sainseq->roundtable == NULL);
  gt_sain_special_singleSinduction1(sainseq,
                                    suftab,
                                    (GtSsainindextype)
                                    (sainseq->totallength-1));
  if (sainseq->seqtype == GT_SAIN_ENCSEQ)
  {
    gt_sain_induceStypes1fromspecialranges(sainseq,
                                           sainseq->seq.encseq,
                                           suftab);
  }
  for (suftabptr = suftab + nonspecialentries - 1; suftabptr >= suftab;
       suftabptr--)
  {
    if ((position = *suftabptr) > 0)
    {
      GtUword currentcc = #{getc_call(key,"position")};

      if (currentcc < sainseq->numofchars)
      {
        GtUword leftcontextcc;

        position--;
        leftcontextcc = #{getc_call(key,"position")};
        GT_SAINUPDATEBUCKETPTR(currentcc);
        gt_assert(bucketptr != NULL && bucketptr - 1 < suftabptr);
        *(--bucketptr) = (leftcontextcc > currentcc)
                          ? ~(position+1) : position;
#ifdef SAINSHOWSTATE
        printf("S-induce: suftab[" GT_WU "]=" GT_WD "\\n",
               (GtUword) (bucketptr - suftab),*bucketptr);
#endif
      }
      *suftabptr = 0;
    }
  }
}

static void gt_sain_#{key}_induceLtypesuffixes2(const GtSainseq *sainseq,
                                                 #{getc_param(key)},
                                         GtSsainindextype *suftab,
                                         GtUword nonspecialentries)
{
  GtUword lastupdatecc = 0;
  GtUsainindextype *fillptr = sainseq->bucketfillptr;
  GtSsainindextype *suftabptr, *bucketptr = NULL;
  #{declare_tmpcc(key,"GtSsainindextype position")}

  for (suftabptr = suftab; suftabptr < suftab + nonspecialentries; suftabptr++)
  {
    position = *suftabptr;
    *suftabptr = ~position;
    if (position > 0)
    {
      GtUword currentcc;

      position--;
      currentcc = #{getc_call(key,"position")};
      if (currentcc < sainseq->numofchars)
      {
        gt_assert(currentcc > 0);
        GT_SAINUPDATEBUCKETPTR(currentcc);
        gt_assert(bucketptr != NULL && suftabptr < bucketptr);
        *bucketptr++ = (position > 0 &&
                        (#{getc_call(key,"position-1")})
                                            < currentcc)
                        ? ~position : position;
#ifdef SAINSHOWSTATE
        gt_assert(bucketptr != NULL);
        printf("L-induce: suftab[" GT_WU "]=" GT_WD "\\n",
               (GtUword) (bucketptr-1-suftab),*(bucketptr-1));
#endif
      }
    }
  }
}

static void gt_sain_#{key}_induceStypesuffixes2(const GtSainseq *sainseq,
                                                 #{getc_param(key)},
                                         GtSsainindextype *suftab,
                                         GtUword nonspecialentries)
{
  GtUword lastupdatecc = 0;
  GtUsainindextype *fillptr = sainseq->bucketfillptr;
  GtSsainindextype *suftabptr, *bucketptr = NULL;
  #{declare_tmpcc(key,"GtSsainindextype position")}

  gt_sain_special_singleSinduction2(sainseq,
                                    suftab,
                                    (GtSsainindextype) sainseq->totallength,
                                    nonspecialentries);
  if (sainseq->seqtype == GT_SAIN_ENCSEQ)
  {
    gt_sain_induceStypes2fromspecialranges(sainseq,
                                           sainseq->seq.encseq,
                                           suftab,
                                           nonspecialentries);
  }
  for (suftabptr = suftab + nonspecialentries - 1; suftabptr >= suftab;
       suftabptr--)
  {
    if ((position = *suftabptr) > 0)
    {
      GtUword currentcc;

      position--;
      currentcc = #{getc_call(key,"position")};
      if (currentcc < sainseq->numofchars)
      {
        GT_SAINUPDATEBUCKETPTR(currentcc);
        gt_assert(bucketptr != NULL && bucketptr - 1 < suftabptr);
        *(--bucketptr) = (position == 0 ||
                          (#{getc_call(key,"position-1")}) > currentcc)
                         ? ~position : position;
#ifdef SAINSHOWSTATE
        gt_assert(bucketptr != NULL);
        printf("S-induce: suftab[" GT_WU "]=" GT_WD "\\n",
                (GtUword) (bucketptr-suftab),*bucketptr);
#endif
      }
    } else
    {
      *suftabptr = ~position;
    }
  }
}

static void gt_sain_#{key}_expandorder2original(GtSainseq *sainseq,
                                                 #{getc_param(key)},
                                         GtUword numberofsuffixes,
                                         GtUsainindextype *suftab)
{
  GtUsainindextype *suftabptr,
                   position,
                   *sstarsuffixes = suftab + GT_MULT2(numberofsuffixes),
                   *sstarfirstcharcount = NULL,
                   *bucketsize = NULL;
  GtUword nextcc = GT_UNIQUEINT(sainseq->totallength);
  #{declare_tmpcc(key,"bool nextisStype = true")}

  if (sainseq->seqtype == GT_SAIN_INTSEQ)
  {
    GtUword charidx;

    gt_assert(sainseq->sstarfirstcharcount == NULL);
    sstarfirstcharcount = sainseq->sstarfirstcharcount
                        = sainseq->bucketfillptr;
    bucketsize = sainseq->bucketsize;
    for (charidx = 0; charidx < sainseq->numofchars; charidx++)
    {
      sstarfirstcharcount[charidx] = 0;
      bucketsize[charidx] = 0;
    }
  }
#{sstarfirstcharcount_bucketsize_assert_not_NULL(key,["INTSEQ"])}
  for (position = (GtUsainindextype) (sainseq->totallength-1); /* Nothing */;
       position--)
  {
    GtUword currentcc = #{getc_call(key,"position")};
    bool currentisStype = (currentcc < nextcc ||
                           (currentcc == nextcc && nextisStype)) ? true : false;

    if (!currentisStype && nextisStype)
    {
#{sstarfirstcharcount_update(key,false,["INTSEQ"])}
      *--sstarsuffixes = position+1;
    }
#{bucketsize_update(key,["INTSEQ"])}
    nextisStype = currentisStype;
    nextcc = currentcc;
    if (position == 0)
    {
      break;
    }
  }
  for (suftabptr = suftab; suftabptr < suftab + numberofsuffixes; suftabptr++)
  {
    *suftabptr = sstarsuffixes[*suftabptr];
  }
}
CCODE
end
