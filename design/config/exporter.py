# -*- coding:utf-8 -*-

import sys
import xlrd

from excel_config import Config


def dump_server_list(table, conf):
    desc = table.row_values(0)      # 属性、
    type = table.row_values(1)      # INT STRING UINT
    side = table.row_values(2)      # s c sc
    # print header
    header = conf.get("svr_header")
    if header:
        print(header)
    print("{")
    # print desc    
    print("    --", end='')
    for j in range(len(type)):
        title = desc[j]
        duo = len(bytes(title, encoding="GBK"))
        if "s" in side[j]:
            if type[j] == "STRING":
                print(desc[j]+" "*(16-duo), end='')
            elif type[j] == "INT":
                print(desc[j]+" "*(16-duo), end='')
            elif type[j] == "FLOAT":
                print(desc[j]+" "*(16-duo), end='')
            else:
                raise Exception("INVALID TYPE")

    print()
    # print content
    for i in range(3, table.nrows):
        row = table.row_values(i)
        if str(row[0])[0] == '#':
            continue
        print("    {", end=' ')
        for j in range(len(type)):
            if "s" in side[j]:
                if type[j] == "STRING":
                    print('{:<16}'.format(str(row[j])+','), end='')
                elif type[j] == "INT":
                    print('{:<16}'.format(str(int(row[j]))+','), end='')
                elif type[j] == "FLOAT":
                    print('{:<16}'.format(str('{:.2f}'.format(float(row[j])))+',' ), end='')
                else:
                    raise Exception("INVALID TYPE")
        print("},")
    print("}")

def dump_server_record(table, conf):
    header = conf.get("svr_header")
    if header:
        print(header)
    print("{")
    for i in range(0, table.nrows):
        row = table.row_values(i)
        print("    ", end='')
        print(str(row[0]), end='')
        _len = 8 - len(row[0])
        print(" "*_len, end='')
        print(" = ", end='')

        val = ""
        type = str(row[1])
        if type == "STRING":
            val = "{}".format(str(row[2]))
        elif type == "INT":
            val = "{}".format(int(row[2]))
        elif type == "FLOAT":
            val = "{:.2f}".format(float(row[2]))
        else:
            raise Exception("INVALID TYPE")
        print(val, end='')
        print(",\t\t", end='')
        if len(row) > 3 and len(row[3]):
            print("-- ", end='')
            print("{}".format(str(row[3])), end='')
        print()
    print("}")


def dump_server(table, conf):
    if conf.get("svr_list"):
        dump_server_list(table, conf)
    else:
        dump_server_record(table, conf)


def dump_client(table, conf):
    pass


def export_sheet(fn, sn, conf):
    str_file = "excel/{0}.xlsx".format(fn)
    data = xlrd.open_workbook(str_file)
    table = data.sheet_by_name(sn)
    if conf.get("server"):
        dump_server(table, conf)
    if conf.get("client"):
        dump_client(table, conf)


def main(argv):
    if len(argv) < 3:
        print("参数数目必须为3")
        return
    fname = argv[1]
    sheet = argv[2]
    excel = Config.get(fname)
    if not excel:
        print("文件名'{0}'未配置".format(fname))
        return
    conf = excel.get(sheet)
    if not conf:
        print("sheet名'{0}'未配置".format(sheet))
        return
    export_sheet(fname, sheet, conf)


if __name__ == "__main__":
    main(sys.argv)
