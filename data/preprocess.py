#!/usr/bin/env python
import csv
import sys
import random
import pandas as pd

def some(x, n):
    return x.ix[random.sample(x.index, n)]

def create_train():
    print('\t\tCreate TRAIN set!')
    # read file
    df_train = pd.read_csv('sets/train_raw/code_list2__train.csv')

    # print(df_train)

    df_train.columns = ['1',  '2',  '3',  '4',
                       '5',  '6',  '7',  '8',
                       '9',  '10', '11', '12',
                       '13', '14', '15', '16',
                       '17', '18', '19', '20',
                       '21', '22', '23', '24',
                       '25', '26', '27', '28',
                       '29', '30', '31', '32',
                       '33', '34']


    columns = ['1',  '2',  '3',  '4',
               '5',  '6',  '7',  '8',
               '9',  '10', '11', '12',
               '13', '14', '15', '16',
               '17', '18', '19', '20',
               '21', '22', '23', '24',
               '25', '26', '27', '28',
               '29', '30', '31', '32',
               '33']

    # print(df_train)
    # df1 = df_test.drop_duplicates()

    df_click = df_train[df_train['34'] == 1]

    df_no_click = df_train[df_train['34'] == 0]

    # df_filtered = df1[df1['34'] == 1]

    # df1.to_csv('test.csv', mode='w', index=False)

    df_click = df_click.drop_duplicates()
    rows_click = df_click.shape[0]
    print('clicks shape:{}'.format(df_click.shape))

    df_no_click = df_no_click.drop_duplicates()
    rows_noclick = df_no_click.shape[0]
    rows_click = df_click.shape[0]

    rows_noclick = 60306 - rows_click;
    # df_no_click = df_no_click.sample(n=rows_noclick)
    df_no_click = df_no_click.sample(frac=0.29)
    # Randomly sample 70% of your dataframe
    # df_no_click = df_no_click.sample(frac=0.15)

    print('no clicks shape:{}'.format( df_no_click.shape))

    frames = [df_click,df_no_click]

    df = pd.concat(frames)

    rows = df.shape[0]

    persent = 100 * rows_click / rows;
    print('rows {0}, rows withs clicks {1}'.format(rows, rows_click))
    print('persent = {}%'.format(persent))


    print('do:{}'.format(df.shape))

    # df.columns = ['1',  '2',  '3',  '4',
    #                    '5',  '6',  '7',  '8',
    #                    '9',  '10', '11', '12',
    #                    '13', '14', '15', '16',
    #                    '17', '18', '19', '20',
    #                    '21', '22', '23', '24',
    #                    '25', '26', '27', '28',
    #                    '29', '30', '31', '32',
    #                    '33', '34']


    df1= df.drop_duplicates(['1',  '2',  '3',  '4',
                       '5',  '6',  '7',  '8',
                       '9',  '10', '11', '12',
                       '13', '14', '15', '16',
                       '17', '18', '19', '20',
                       '21', '22', '23', '24',
                       '25', '26', '27', '28',
                       '29', '30', '31', '32',
                       '33'])



    # df1.sort_index(inplace=True)

    df1 = df1.sample(n=df1.shape[0])
    print('posle:{}'.format(df1.shape))

    # df_click.to_csv('sets/train/clicks_pBP0Dr1q_7days.csv', mode='w',header=False, index=False)
    df_click.to_csv('sets/train/ccode_list2__train_click.csv', mode='w', header=False, index=False)
    # df_no_click.to_csv('no_click.csv', mode='w',header=False, index=False)

    df1.to_csv('sets/train/code_list2__train.csv', mode='w',header=False, index=False)

def create_test():
    print('\t\tCreate TEST set!')
    # read file
    df_train = pd.read_csv('sets/test_raw/code_list.csv')

    df_train.columns = ['1', '2', '3', '4',
                        '5', '6', '7', '8',
                        '9', '10', '11', '12',
                        '13', '14', '15', '16',
                        '17', '18', '19', '20',
                        '21', '22', '23', '24',
                        '25', '26', '27', '28',
                        '29', '30', '31', '32',
                        '33', '34']


    df_click = df_train[df_train['34'] == 1]
    df_no_click = df_train[df_train['34'] == 0]

    print('clicks shape do:{}'.format(df_click.shape))
    df_click = df_click.drop_duplicates()
    rows_click = df_click.shape[0]
    print('clicks shape:{}'.format(df_click.shape))

    print('no clicks shape:{}'.format(df_no_click.shape))
    df_no_click = df_no_click.drop_duplicates()
    print('no clicks shape:{}'.format( df_no_click.shape))

    frames = [df_click,df_no_click]

    df = pd.concat(frames)

    rows = df.shape[0]
    persent = 100 * rows_click / rows;
    print('rows {0}, rows withs clicks {1}'.format(rows, rows_click))
    print('persent = {}%'.format(persent))


    print('do:{}'.format(df.shape))


    df1= df.drop_duplicates(['1',  '2',  '3',  '4',
                       '5',  '6',  '7',  '8',
                       '9',  '10', '11', '12',
                       '13', '14', '15', '16',
                       '17', '18', '19', '20',
                       '21', '22', '23', '24',
                       '25', '26', '27', '28',
                       '29', '30', '31', '32',
                       '33'])

    print('posle:{}'.format(df1.shape))

    df1.sort_index(inplace=True)

    df_click.to_csv('sets/test/clicks_test.csv', mode='w',header=False, index=False)
    # df_no_click.to_csv('no_click.csv', mode='w',header=False, index=False)

    df1.to_csv('sets/test/test_without_duplicates_pBP0Dr1q_3days.csv', mode='w',header=False, index=False)

if __name__ == '__main__':
    create_train()
    # create_test()