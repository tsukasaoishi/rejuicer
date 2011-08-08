require 'rejuicer_set'
#
# Rejuicer is easy search engine.
# RejuicerSet is used to set operation.
#
# work = Struct.new(:id, :odd_flag, :remainder_3, :remainder_5)
# mother = (0...10000).inject([]){|m, i| m << work.new(i, i % 2 == 0, i % 3, i % 5)}
#
# index = Rejuicer.new(:odd_flag, :remainder_3, :remainder_5)
# index.set(mother)
#
# index.search(:odd_flag => false) #=> [1,3,5,7,9...,9997,9999]
# index.search(:remainder_3 => 2, :remainder_5 => 4) #=> [14,29,44,59,...,9974,9989]
#
class Rejuicer
  VERSION = "0.0.2"

  #
  # args :
  #   methods or attributes name
  #   that index target
  #
  def initialize(*args)
    @base = RejuicerSet.new
    @tree = args.inject({}){|t,i| t[i.to_sym] = {};t}
  end

  #
  # indexing
  #   array : objects
  #   id_attr : index id
  #
  def set(array, id_attr = :id)
    array.each{|a| add(a, id_attr)}
  end

  #
  # indexing
  #   obj : object
  #   id_attr : index id
  #
  def add(obj, id_attr = :id)
    obj_id = obj.__send__(id_attr)
    @base << obj_id
    @tree.keys.each do |k|
      begin
        at = obj.__send__(k)
      rescue NoMethodError
        next
      end

      @tree[k.to_sym][at] ||= RejuicerSet.new
      @tree[k.to_sym][at] << obj_id
    end
  end
  alias :<< :add

  #
  # delete from index
  #   obj : object
  #   id_attr : index id
  #
  def delete(obj, id_attr = :id)
    obj_id = obj.__send__(id_attr)
    @base.delete(obj_id)
    @tree.keys.each do |k|
      begin
        at = obj.__send__(k)
      rescue NoMethodError
        next
      end

      next unless @tree[k.to_sym][at]
      @tree[k.to_sym][at].delete(obj_id)
    end
  end

  # 
  # search
  #  conditions: search target
  #
  def search(conditions = nil)
    return @base.to_a if conditions.nil? || conditions.empty?

    f_cond = conditions.shift
    f_set = @tree[f_cond.first.to_sym][f_cond.last]
    return f_set.to_a if conditions.empty?

    ret = conditions.sort_by{|cond| @tree[cond.first.to_sym][cond.last].size}.inject(f_set) do |work, cond|
      work & @tree[cond.first.to_sym][cond.last]
    end
    ret.to_a
  end
end
