#pragma once
#include <vector>
#include <memory>
#include <functional>

template <typename T>
class Pool
{
public:
    
	/// @brief コンストラクタ
    /// @param generator 
    Pool(std::function<std::unique_ptr<T>()> generator)
        : generator_(generator) {
    }

    
	/// @brief 事前にオブジェクトを生成してプールに格納する
    /// @param count 
    void PreAllocate(int count)
    {
        for (int i = 0; i < count; ++i) 
        {
            pool_.push_back(generator_());
        }
    }

	/// @brief オブジェクトをプールから取得する
    /// @return 
    std::unique_ptr<T> Acquire()
    {
        // プールが空っぽ（事前生成した分を使い切った）場合
        if (pool_.empty())
        {
            // ここで自動的に新規生成して返す（動的拡張）
            return generator_();
        }

        // プールに余りがあれば、末尾から1つ取り出して返す
        std::unique_ptr<T> obj = std::move(pool_.back());
        pool_.pop_back();
        return obj;
    }

	/// @brief オブジェクトをプールに返却する
    /// @param obj 
    void Release(std::unique_ptr<T> obj)
    {
        pool_.push_back(std::move(obj));
    }

	/// @brief カウントを取得する
	/// @return 
	int GetCount() const { return count_; }

	/// @brief カウントを設定する
	/// @param count 
	void SetCount(int count) { count_ = count; }

private:

	// プールの内部データ構造
    std::vector<std::unique_ptr<T>> pool_;

	// オブジェクト生成用の関数オブジェクト
    std::function<std::unique_ptr<T>()> generator_;

	/// @brief カウント用
	int count_ = 0;
};