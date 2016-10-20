	nrows = table.nrows
    ncols = table.ncols

    print(nrows, ncols)


    print( table.row_values(2) )
    print( table.col_values(2) )


    print("-------------------")

    for i in range(nrows):
        print( table.row_values(i) )


    print("-------------------")

    cell_A1 = table.cell(1,0).value
    cell_C4 = table.cell(2,2).value
    print(cell_A1 , cell_C4)

    cell_A1 = table.row(3)[2].value
    cell_A2 = table.col(1)[3].value
    print(cell_A1 , cell_A2)
